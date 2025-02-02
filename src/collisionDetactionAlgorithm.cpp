#include <Arduino.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "collisionDetectionAlgorithm.h"

// Vibration pattern for collision alert
void vibrateMotorsAsTask(void *pvParameters) {
    // Extract parameters
    void** params = (void**)pvParameters;
    vibrationMotor* motor = (vibrationMotor*)params[0];
    String* pattern = (String*)params[1];
    motor->vibrateFromPatternAsstring(*pattern);
    vTaskDelay(1000);
    vTaskDelete(nullptr);
}
// Play MP3 file as a task for collision alert
void playMP3AsTask(void *pvParameters) {
  // Cast the incoming parameter to an array of void pointers
  void** params = (void**) pvParameters;

  MP3* mp3 = (MP3*) params[0];
  uint8_t directory_name = (uint8_t)(uintptr_t)params[1];
  uint8_t file_name  = (uint8_t)(uintptr_t)params[2];

  mp3->playWithFileName(directory_name, file_name);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

// This task calculates the user's velocity based on the step count
void calculateVelocityAsTask(void *pvParameters) {
    VelocityTaskParams *params = (VelocityTaskParams *)pvParameters;
    int delay_in_ms = params->delay_in_ms;
    systemSettings *settings = params->settings;
    double *velocity = params->velocity;
    int *step_count = params->step_count;
    const SensorData &sensor_data = *(params->sensor_data); // Use const reference
    bool is_system_on = *(params->system_on_flag);
    float user_height_in_meters = settings->getUserHeight() / 100.0f;
    while (true) {
        if (is_system_on) {
            calculateStepCountAndSpeed(sensor_data, step_count, velocity, user_height_in_meters);
        }
        vTaskDelay(delay_in_ms);
    }
}

/* Calculate the velocity based on the acceleration data
    How does it work?
    We count the user's steps and calculate the speed based on the step frequency and stride length.
    The step detection logic uses both acceleration and gyroscope variation to detect steps.
    The speed is calculated every SPEED_WINDOW_MS milliseconds.
    The stride length is estimated as 0.415 times the user's height in meters for walking.
    The step frequency is calculated as the number of steps in the window divided by the window duration.
    The speed is estimated as the step frequency times the stride length.
    The step detection thresholds can be adjusted based on the sensor data and user behavior.
    The gyroscope threshold can be used to filter out small movements that are not steps.
    The step time threshold can be used to prevent double-counting steps that are too close together.
    The speed window duration can be adjusted to balance responsiveness and accuracy.
    The user height can be used to estimate the stride length for different users.
*/
void calculateStepCountAndSpeed(const SensorData& sensorData, int* stepCount, double* speed, float userHeight) {
    static bool isStepDetected = false;
    static float prevAccX = 0.0f;          // Previous X-axis acceleration
    static float prevGyroMagnitude = 0.0f; // Previous gyroscope magnitude
    static unsigned long lastStepTime = 0; // Time of the last detected step
    static unsigned long startTime = 0;    // Start time of the step frequency measurement window
    static int stepsInWindow = 0;          // Steps counted in the current window
 
    // Get the current smoothed acceleration along the X-axis
    float currentAccX = sensorData.getLinearAccelX();

    // Calculate the delta (change) in acceleration
    float deltaAccX = fabs(currentAccX - prevAccX);

    // Get the gyroscope readings
    float gyroX = sensorData.getGyroX();
    float gyroY = sensorData.getGyroY();
    float gyroZ = sensorData.getGyroZ();

    // Calculate the magnitude of the gyroscope vector
    float currentGyroMagnitude = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

    // Calculate the delta (change) in gyroscope magnitude
    float deltaGyroMagnitude = fabs(currentGyroMagnitude - prevGyroMagnitude);

    // Get current time
    unsigned long currentTime = millis();

    // Step detection logic using both acceleration and gyroscope variation
    if (!isStepDetected && deltaAccX > STEP_HIGH_THRESHOLD && deltaGyroMagnitude > GYRO_THRESHOLD &&
        (currentTime - lastStepTime) > STEP_TIME_THRESHOLD) {
        // Step peak detected
        isStepDetected = true;
        *stepCount += 1;    // Increment total step count
        stepsInWindow += 1; // Increment steps in the current window
        lastStepTime = currentTime;

        //Serial.println("Step detected (with gyroscope)");
    } else if (isStepDetected && deltaAccX < STEP_LOW_THRESHOLD && deltaGyroMagnitude < GYRO_THRESHOLD) {
        // Reset state after step detected
        isStepDetected = false;
    }

    // Calculate speed every SPEED_WINDOW_MS
    if (currentTime - startTime >= SPEED_WINDOW_MS) {
        // Estimate stride length (roughly 0.415 times the user's height in meters for walking)
        float strideLength = userHeight * ESTIMATED_STRIDE_LENGTH; 

        // Calculate step frequency (steps per second)
        float stepFrequency = stepsInWindow / (SPEED_WINDOW_MS / 1000.0);

        // Estimate speed (meters per second)
        *speed = stepFrequency * strideLength;
        
        // log data
        String log_data = "INFO: User height: " + String(userHeight) + ", Stride length: " + String(strideLength) + ", Steps in window: " + String(stepsInWindow) + ", Speed estimated: " + String(*speed) + " m/s";
        logData(log_data);

        // Reset for the next window
        stepsInWindow = 0;
        startTime = currentTime;
    }

    // Update previous values
    prevAccX = currentAccX;
    prevGyroMagnitude = currentGyroMagnitude;

    //log data
    String log_data = "INFO: Delta AccX: " + String(deltaAccX) + ", Delta Gyro Magnitude: " + String(deltaGyroMagnitude) + ", Step Count: " + String(*stepCount);
    logData(log_data);
}

double nearestObstacleCollisionTime(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity) {
    // Static variable to store the previous x_distance
    static int previous_x_distance = -1; // Initialize with an invalid value

    // User and system heights
    double user_height_in_mm = system_settings.getUserHeight() * 10; // Height of user in mm
    double system_height_in_mm = system_settings.getSystemHeight() * 10; // Height of the system in mm
    double minimun_obstacle_height_in_mm = system_settings.getMinimumObstacleHeight() * 10; // Minimum obstacle height in mm
    double head_clearance_in_mm = system_settings.getHeadClearance() * 10; // Head clearance in mm
    double impact_time = 0.0;

    // Calculate the height of the user's head
    double user_head_height = user_height_in_mm - system_height_in_mm;
    user_head_height += head_clearance_in_mm; // add the head clearance to the user's head height

    // Store distances (X, Z) in a vector for sorting
    std::vector<std::pair<int, int>> distances;

    double pitch_value = 0; //sensor_data.getPitch();

    // Calculate and store distances for each sensor
    distances.push_back({
        sensor_data.getDistanceSensor1() * cos((abs(SENSOR_1_ANGLE + pitch_value)) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor1() * sin((abs(SENSOR_1_ANGLE + pitch_value)) * (M_PI / 180.0)) + SENSOR_1_BOX_HEIGHT)
    });

    distances.push_back({
        sensor_data.getDistanceSensor2() * cos((abs(SENSOR_2_ANGLE + pitch_value)) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor2() * sin((abs(SENSOR_2_ANGLE + pitch_value)) * (M_PI / 180.0)) + SENSOR_2_BOX_HEIGHT)
    });

    distances.push_back({
        sensor_data.getDistanceSensor3() * cos((abs(SENSOR_3_ANGLE + pitch_value)) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor3() * sin((abs(SENSOR_3_ANGLE + pitch_value)) * (M_PI / 180.0)) + SENSOR_3_BOX_HEIGHT)
    });

    distances.push_back({
        sensor_data.getDistanceSensor4() * cos((abs(SENSOR_4_ANGLE + pitch_value)) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor4() * sin((abs(SENSOR_4_ANGLE + pitch_value)) * (M_PI / 180.0))) + SENSOR_4_BOX_HEIGHT
    });

    // Sort distances by X (ascending)
    std::sort(distances.begin(), distances.end());

    // Flag to track if any valid obstacle is detected
    bool found_valid_obstacle = false;

    // Process each distance
    for (const auto& distance : distances) {
        int x_distance = distance.first;
        int z_distance = distance.second;

        // Ignore distances where Z is higher than the user's head or X is 0
        if (x_distance <= 0 || z_distance > user_head_height ||  z_distance < minimun_obstacle_height_in_mm) {
            //log data
            String log_data = "INFO: Obstacle detected but ignored (above user's head, below minimum height, or at x = 0): X_distance=" + String(x_distance) + ", Z_distance=" + String(z_distance) + ", User head height=" + String(user_head_height) + ", Minimum obstacle height=" + String(minimun_obstacle_height_in_mm);
            logData(log_data);
            continue;
        }

        // Check if we are walking toward the obstacle
        if (previous_x_distance == -1 || (previous_x_distance - x_distance) > DISTANCE_CHANGE_THRESHOLD) {
            if (*velocity <= 0) {
                //Serial.println("Velocity is zero or negative; cannot calculate impact time.");
            } else {
                impact_time = (x_distance / 1000.0) / *velocity;
                //log data
                String log_data = "INFO: Obstacle detected. X_distance: " + String(x_distance) + ", Z_distance: " + String(z_distance) + ", Expected impact time: " + String(impact_time);
                logData(log_data);   

                previous_x_distance = x_distance; // Update the previous distance
                found_valid_obstacle = true;
                return impact_time;
            }
        } else {
            //log data
            String log_data = "INFO: Obstacle detected but user not walking toward it. X_distance=" + String(x_distance) + ", Previous X_distance=" + String(previous_x_distance);
            logData(log_data);
        }
    }

    // Reset previous_x_distance only if no valid obstacles are detected
    if (!found_valid_obstacle) {
        previous_x_distance = -1;
    }
    return 0; // No valid obstacle detected
}

double distanceToNearestObstacle(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity, bool mpu_degraded_flag) {
    static int previous_x_distance = -1; // Initialize with an invalid value

    double user_height_in_mm = system_settings.getUserHeight() * 10; // Height of user in mm
    double system_height_in_mm = system_settings.getSystemHeight() * 10; // Height of the system in mm

    double minimun_obstacle_height_in_mm = system_settings.getMinimumObstacleHeight() * 10; // Minimum obstacle height in mm
    double head_clearance_in_mm = system_settings.getHeadClearance() * 10; // Head clearance in mm

    // Calculate the height of the user's head
    double user_head_height = user_height_in_mm - system_height_in_mm;
    user_head_height += head_clearance_in_mm; // add the head clearance to the user's head height

    std::vector<std::pair<int, int>> distances;
    double pitch_value = 0; // mpu_degraded_flag ? 0 : sensor_data.getPitch();

    // Calculate and store distances for each sensor
    distances.push_back({
        sensor_data.getDistanceSensor1() * cos((SENSOR_1_ANGLE + pitch_value) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor1() * sin((SENSOR_1_ANGLE + pitch_value) * (M_PI / 180.0)) + SENSOR_1_BOX_HEIGHT)
    });

    distances.push_back({
        sensor_data.getDistanceSensor2() * cos((SENSOR_2_ANGLE + pitch_value) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor2() * sin((SENSOR_2_ANGLE + pitch_value) * (M_PI / 180.0)) + SENSOR_2_BOX_HEIGHT)
    });

    distances.push_back({
        sensor_data.getDistanceSensor3() * cos((SENSOR_3_ANGLE + pitch_value) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor3() * sin((SENSOR_3_ANGLE + pitch_value) * (M_PI / 180.0)) + SENSOR_3_BOX_HEIGHT)
    });

    distances.push_back({
        sensor_data.getDistanceSensor4() * cos((SENSOR_4_ANGLE + pitch_value) * (M_PI / 180.0)),
        (sensor_data.getDistanceSensor4() * sin((SENSOR_4_ANGLE + pitch_value) * (M_PI / 180.0))) + SENSOR_4_BOX_HEIGHT
    });

    // Sort distances by X (ascending)
    std::sort(distances.begin(), distances.end());

    bool found_valid_obstacle = false;

    for (const auto& distance : distances) {
        int x_distance = distance.first;
        int z_distance = distance.second;

        // Ignore obstacles above the user's head or at X=0
        if (x_distance == 0 || z_distance > user_head_height || z_distance < minimun_obstacle_height_in_mm) {
            //log data
            String log_data = "INFO: Obstacle detected but ignored (above user's head, below minimum height or at x = 0): X_distance=" + String(x_distance) + ", Z_distance=" + String(z_distance) + ", User head height=" + String(user_head_height) + ", Minimum obstacle height=" + String(minimun_obstacle_height_in_mm);
            continue;
        }

        // Print previous and current x_distance for debugging
        Serial.print("Previous x_distance: ");
        Serial.println(previous_x_distance);
        Serial.print("Current x_distance: ");
        Serial.println(x_distance);

        // Check if the user is approaching the obstacle
        if (previous_x_distance == -1 || (previous_x_distance - x_distance) > DISTANCE_CHANGE_THRESHOLD) {
            if (abs(previous_x_distance - x_distance) < DISTANCE_CHANGE_THRESHOLD) {
                String log_data = "INFO: Condition: Ignored - Static obstacle detected; no alert triggered.";
                continue;
            }

            Serial.println("Condition: Alert - User approaching obstacle.");
            Serial.print("Previous x_distance: ");
            Serial.println(previous_x_distance);
            Serial.print("x_distance: ");
            Serial.println(x_distance);

            previous_x_distance = x_distance; // Update the previous distance
            found_valid_obstacle = true;
            return x_distance;
        } else {
            Serial.println("Condition: Ignored - User not approaching obstacle.");
            Serial.print("Previous x_distance: ");
            Serial.println(previous_x_distance);
            Serial.print("Current x_distance: ");
            Serial.println(x_distance);
        }
    }

    // Reset previous_x_distance only if no valid obstacles are detected
    /*if (!found_valid_obstacle) {
        Serial.println("Condition: No valid obstacle found. Resetting previous_x_distance.");
        previous_x_distance = -1;
    }*/
    return 0;
}

bool collisionTimeAlertHandler(double collision_time, systemSettings& system_settings, const MP3& mp3, vibrationMotor& motor1) {
    if (collision_time > 0) {
      if (system_settings.getEnableAlert1() && !system_settings.getEnableAlert2() && !system_settings.getEnableAlert3()) {
        if (collision_time <= system_settings.getAlertTiming1()) {
          String log_data = "ALERT: Alerted collision from alert 1";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration1(), system_settings.getAlertSound1AsInt());
          return true;
        }
      }
      if (system_settings.getEnableAlert1() && system_settings.getEnableAlert2() && !system_settings.getEnableAlert3()) {
        if (collision_time <= system_settings.getAlertTiming1() && collision_time > system_settings.getAlertTiming2()) {
          String log_data = "ALERT: Alerted collision from alert 1";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration1(), system_settings.getAlertSound1AsInt());
          return true;
        }
        if (collision_time <= system_settings.getAlertTiming2() && collision_time > 0) {
          String log_data = "ALERT: Alerted collision from alert 2";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration2(), system_settings.getAlertSound2AsInt());
          return true;
        }
      }
      if (system_settings.getEnableAlert1() && system_settings.getEnableAlert2() && system_settings.getEnableAlert3()) {
        if (collision_time <= system_settings.getAlertTiming1() && collision_time > system_settings.getAlertTiming2()) {
          String log_data = "ALERT: Alerted collision from alert 1";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration1(), system_settings.getAlertSound1AsInt());
          return true;
        }
        if (collision_time <= system_settings.getAlertTiming2() && collision_time > system_settings.getAlertTiming3()) {
          String log_data = "ALERT: Alerted collision from alert 2";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration2(), system_settings.getAlertSound2AsInt());
          return true;
        }
        if (collision_time > 0 && collision_time <= system_settings.getAlertTiming3()) {
          String log_data = "ALERT: Alerted collision from alert 3";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration3(), system_settings.getAlertSound3AsInt());
          return true;
        }
      }
    }
    return false;
}

bool obstacleDistanceAlertHandler(double obstacle_distance, systemSettings& system_settings, const MP3& mp3, vibrationMotor& motor1) {
    if (obstacle_distance > 0) {
      if (system_settings.getEnableAlert1() && !system_settings.getEnableAlert2() && !system_settings.getEnableAlert3()) {
        if (obstacle_distance <= system_settings.getAlertDistance1()*10){
          String log_data = "ALERT: Alerted collision from alert 1";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration1(), system_settings.getAlertSound1AsInt());
          return true;
        }
      }
      if (system_settings.getEnableAlert1() && system_settings.getEnableAlert2() && !system_settings.getEnableAlert3()) {
        if (obstacle_distance <= system_settings.getAlertDistance1()*10 && obstacle_distance > system_settings.getAlertDistance2()*10) {
          String log_data = "ALERT: Alerted collision from alert 1";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration1(), system_settings.getAlertSound1AsInt());
          return true;
        }
        if (obstacle_distance <= system_settings.getAlertDistance2()*10 && obstacle_distance > 0) {
          String log_data = "ALERT: Alerted collision from alert 2";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration2(), system_settings.getAlertSound2AsInt());
          return true;
        }
      }
      if (system_settings.getEnableAlert1() && system_settings.getEnableAlert2() && system_settings.getEnableAlert3()) {
        if (obstacle_distance <= system_settings.getAlertDistance1()*10 && obstacle_distance > system_settings.getAlertDistance2()*10) {
          String log_data = "ALERT: Alerted collision from alert 1";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration1(), system_settings.getAlertSound1AsInt());
          return true;
        }
        if (obstacle_distance <= system_settings.getAlertDistance2()*10 && obstacle_distance > system_settings.getAlertDistance3()*10) {
          String log_data = "ALERT: Alerted collision from alert 2";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration2(), system_settings.getAlertSound2AsInt());
          return true;
        }
        if (obstacle_distance > 0 && obstacle_distance <= system_settings.getAlertDistance3()*10) {
          String log_data = "ALERT: Alerted collision from alert 3";
          logData(log_data);
          collisionAlert(system_settings, mp3, motor1, system_settings.getAlertVibration3(), system_settings.getAlertSound3AsInt());
          return true;
        }
      }
    }
    return false;
}

void collisionAlert(const systemSettings& system_settings, const MP3& mp3, vibrationMotor& vibration_motor, String vib_pattern, uint alert_sound_type) {

    static void* audio_params[3];
    audio_params[0] = (void*)&mp3;                  
    audio_params[1] = (void*)(uintptr_t)ALERTS_DIR; //dir name
    audio_params[2] = (void*)(uintptr_t)alert_sound_type;       //file name

    // Parameters for vibration task
    static void* vibration_params[2];
    vibration_params[0] = (void*)&vibration_motor;          
    vibration_params[1] = (void*)&vib_pattern; 
    if(system_settings.getMode() == "Vibration") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, &vibration_params, 4, nullptr);
        vTaskDelay(1500);
    }
    if(system_settings.getMode() == "Sound") {
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1500);
    }
    if(system_settings.getMode() == "Both") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, &vibration_params, 4, nullptr);  
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1500);
    }
}

// Samples sensors data
void sampleSensorsData(void *pvParameters) {
    SampleSensorDataParams *params = (SampleSensorDataParams *)pvParameters;
    int delay_in_ms = params->delay_in_ms;
    bool is_system_on = *(params->system_on_flag);
    bool mpu_degraded_flag = *(params->mpu_deg_flag);
    std::vector<std::pair<Adafruit_VL53L1X *, int>> distance_sensors = params->distance_sensors_vector;
    std::vector<std::pair<MPU9250 *, int>> mpu_sensors = params->mpu_sensors_vector;
    SensorData &sensor_data = *(params->sensor_data);
    MP3& mp3 = *(params->mp3_pointer);

    int distance = 0;
    bool distance_sensor_degraded_notification_flag = true;
    bool all_distance_sensors_degraded_notification_flag = true;
    bool sensor_1_unplugged = false;
    bool sensor_2_unplugged = false;
    bool sensor_3_unplugged = false; 
    bool sensor_4_unplugged = false;
    
    while (true)
    {
        if (is_system_on)
        {
            // samples distance sensors data
            for (int i = 0; i < distance_sensors.size(); i++)
            {
                if (!isVL53L1XSensorConnected(distance_sensors[i].second, &Wire)) {
                    Serial.print("Sensor: ");
                    Serial.print(i + 1);
                    Serial.println(" not connected");
                    distance = -1;
                    if(i == 0) {
                        sensor_1_unplugged = true;
                    }
                    if(i == 1) {
                        sensor_2_unplugged = true;
                    }
                    if(i == 2) {
                        sensor_3_unplugged = true;
                    }
                    if(i == 3) {
                        sensor_4_unplugged = true;
                    }
                    if (distance_sensor_degraded_notification_flag) {
                        mp3.playWithFileName(VOICE_ALERTS_DIR, DISTANCE_SENSOR_DEGRADED);
                        vTaskDelay(4000);
                        String log_data = "ERROR: ONE OR MORE DISTANCE MEASURING SENSORS NOT CONNECTED - OPERATING IN DEGRADED MODE";
                        logData(log_data);
                        distance_sensor_degraded_notification_flag = false;
                    }
                    if(sensor_1_unplugged && sensor_2_unplugged && sensor_3_unplugged && sensor_4_unplugged && all_distance_sensors_degraded_notification_flag) {
                        mp3.playWithFileName(VOICE_ALERTS_DIR, ALL_DISTANCE_MEARUSING_SENSORS_NOT_CONNECTED);
                        vTaskDelay(7000);                        
                        String log_data = "ERROR: ALL DISTANCE MEASURING SENSORS NOT CONNECTED - OPERATING IN DEGRADED MODE";
                        logData(log_data);
                        all_distance_sensors_degraded_notification_flag = false;
                    }
                } else {
                    vTaskDelay(50);
                    if (distance_sensors[i].first->dataReady()) {
                        distance = distance_sensors[i].first->distance();
                    } else {
                        Serial.println(F("Data not ready"));
                    }
                }
                if (i == 0) {
                    sensor_data.setSensor1Distance(distance);
                }
                if (i == 1) {
                    sensor_data.setSensor2Distance(distance);
                }
                if (i == 2) {
                    sensor_data.setSensor3Distance(distance);
                }
                if (i == 3) {
                    sensor_data.setSensor4Distance(distance);
                }
                distance_sensors[i].first->clearInterrupt();
            }
            // samples MPU data
            if (!mpu_degraded_flag && mpu_sensors[0].first->update()) {
                vTaskDelay(delay_in_ms);
                sensor_data.setPitch(mpu_sensors[0].first->getPitch());
                int yaw = mpu_sensors[0].first->getYaw();
                if (yaw < 0)
                {
                    yaw = 360 + yaw;
                }
                sensor_data.setYaw(yaw);
                sensor_data.setRoll(mpu_sensors[0].first->getRoll());
                sensor_data.setAccelX(mpu_sensors[0].first->getAccX());
                sensor_data.setAccelY(mpu_sensors[0].first->getAccY());
                sensor_data.setAccelZ(mpu_sensors[0].first->getAccZ());
                sensor_data.setLinearAccelX(mpu_sensors[0].first->getLinearAccX());
                sensor_data.setLinearAccelY(mpu_sensors[0].first->getLinearAccY());
                sensor_data.setLinearAccelZ(mpu_sensors[0].first->getLinearAccZ());
                sensor_data.setGyroX(mpu_sensors[0].first->getGyroX());
                sensor_data.setGyroY(mpu_sensors[0].first->getGyroY());
                sensor_data.setGyroZ(mpu_sensors[0].first->getGyroZ());
                sensor_data.updateLinearAccelX();
            }
            sensor_data.setlastUpdateTime(millis());
        }
    }
    vTaskDelay(delay_in_ms);
}
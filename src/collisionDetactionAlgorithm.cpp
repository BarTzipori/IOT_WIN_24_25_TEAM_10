#include <Arduino.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "collisionDetectionAlgorithm.h"

// Vibration pattern for collision alert
void vibrateMotorsAsTask(void *pvParameters) {
    void** params = (void**) pvParameters;
    vibrationMotor* motor = (vibrationMotor*)params[0];
    String* vib_pattern = (String*)params[1];
    
    motor->vibrateFromPatternAsstring(*vib_pattern);
    vTaskDelete(NULL);
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
  // Task is done, so delete itself
  vTaskDelete(NULL);
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
void calculateStepCountAndSpeed(const SensorData& sensorData, int* stepCount, float* speed, float userHeight) {
    static bool isStepDetected = false;
    static float prevAccX = 0.0f;          // Previous X-axis acceleration
    static float prevGyroMagnitude = 0.0f; // Previous gyroscope magnitude
    static unsigned long lastStepTime = 0; // Time of the last detected step
    static unsigned long startTime = 0;    // Start time of the step frequency measurement window
    static int stepsInWindow = 0;          // Steps counted in the current window
    const unsigned long STEP_TIME_THRESHOLD = 300; // Minimum time between steps in milliseconds
    const unsigned long SPEED_WINDOW_MS = 1000;    // Speed calculation window in milliseconds

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

    // Thresholds for step detection
    const float STEP_HIGH_THRESHOLD = 0.07f;  // Threshold for detecting a step's peak in AccX
    const float STEP_LOW_THRESHOLD = 0.02f;   // Threshold for resetting the step state
    const float GYRO_THRESHOLD = 0.08f;       // Minimum gyroscope change to indicate a step

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

        Serial.println("Step detected (with gyroscope)");
    } else if (isStepDetected && deltaAccX < STEP_LOW_THRESHOLD && deltaGyroMagnitude < GYRO_THRESHOLD) {
        // Reset state after step detected
        isStepDetected = false;
    }

    // Calculate speed every SPEED_WINDOW_MS
    if (currentTime - startTime >= SPEED_WINDOW_MS) {
        // Estimate stride length (roughly 0.415 times the user's height in meters for walking)
        float strideLength = userHeight * 0.415; // Adjust factor for running if needed

        // Calculate step frequency (steps per second)
        float stepFrequency = stepsInWindow / (SPEED_WINDOW_MS / 1000.0);

        // Estimate speed (meters per second)
        *speed = stepFrequency * strideLength;

        Serial.print("Steps in window: ");
        Serial.print(stepsInWindow);
        Serial.print("| Speed estimated: ");
        Serial.print(*speed);
        Serial.println(" m/s");

        // Reset for the next window
        stepsInWindow = 0;
        startTime = currentTime;
    }

    // Update previous values
    prevAccX = currentAccX;
    prevGyroMagnitude = currentGyroMagnitude;

    // Debug output
    Serial.print("Delta AccX: ");
    Serial.print(deltaAccX);
    Serial.print(" | Delta Gyro Magnitude: ");
    Serial.print(deltaGyroMagnitude);
    Serial.print(" | Step Count: ");
    Serial.println(*stepCount);
}

bool collisionDetector(const SensorData& sensor_data, const systemSettings& system_settings, float* velocity) {
    
    //user height
    double user_height_in_mm = system_settings.getUserHeight()*1000;
    double system_height_in_mm = system_settings.getSystemHeight()*1000; //height of the system in mm

    //distance in X and Z from sensor 1
    int x_distance_from_sensor1 = sensor_data.getDistanceSensor1() * cos(SENSOR_1_ANGLE * (M_PI / 180.0));
    int z_distance_from_sensor1 = sensor_data.getDistanceSensor1() * sin(SENSOR_1_ANGLE * (M_PI / 180.0));      
    //distance in X and Z fron semsor 2
    int x_distance_from_sensor2 = sensor_data.getDistanceSensor2() * cos(SENSOR_2_ANGLE * (M_PI / 180.0));
    int z_distance_from_sensor2 = sensor_data.getDistanceSensor2() * sin(SENSOR_2_ANGLE * (M_PI / 180.0));
    //distance in X and Z from sensor 3
    int x_distance_from_sensor3 = sensor_data.getDistanceSensor3() * cos(SENSOR_3_ANGLE * (M_PI / 180.0));
    int z_distance_from_sensor3 = sensor_data.getDistanceSensor3() * sin(SENSOR_3_ANGLE * (M_PI / 180.0));
    //distance in X and Z from sensor 4
    int x_distance_from_sensor4 = sensor_data.getDistanceSensor4() * cos(SENSOR_4_ANGLE * (M_PI / 180.0));
    int z_distance_from_sensor4 = sensor_data.getDistanceSensor4() * sin(SENSOR_4_ANGLE * (M_PI / 180.0));

// Calculate the height of the user's head
    double user_head_height = user_height_in_mm - system_height_in_mm;

    // Store distances (X, Z) in a vector for sorting
    std::vector<std::pair<int, int>> distances;

    // Calculate and store distances for each sensor
    distances.push_back({ 
        sensor_data.getDistanceSensor1() * cos(SENSOR_1_ANGLE * (M_PI / 180.0)), 
        sensor_data.getDistanceSensor1() * sin(SENSOR_1_ANGLE * (M_PI / 180.0)) 
    });

    distances.push_back({ 
        sensor_data.getDistanceSensor2() * cos(SENSOR_2_ANGLE * (M_PI / 180.0)), 
        sensor_data.getDistanceSensor2() * sin(SENSOR_2_ANGLE * (M_PI / 180.0)) 
    });

    distances.push_back({ 
        sensor_data.getDistanceSensor3() * cos(SENSOR_3_ANGLE * (M_PI / 180.0)), 
        sensor_data.getDistanceSensor3() * sin(SENSOR_3_ANGLE * (M_PI / 180.0)) 
    });

    distances.push_back({ 
        sensor_data.getDistanceSensor4() * cos(SENSOR_4_ANGLE * (M_PI / 180.0)), 
        sensor_data.getDistanceSensor4() * sin(SENSOR_4_ANGLE * (M_PI / 180.0)) 
    });

    // Sort distances by X (ascending)
    std::sort(distances.begin(), distances.end());

    // Process each distance
    for (const auto& distance : distances) {
        int x_distance = distance.first;
        int z_distance = distance.second;

        // Ignore distances where Z is higher than the user's head
        if (z_distance > user_head_height) {
            continue;
        } else {
            double impact_time = x_distance / *velocity;
            if(impact_time <= system_settings.getTiming()) {
                Serial.println("Collision detected");
                return true;
            }
        }
    }
    return false;
}

void collisionAlert(const systemSettings& system_settings, const MP3& mp3, vibrationMotor& vibration_motor, String vibration_pattern) {
    Serial.println("Obstacle detected");

    static void* audio_params[3];
    audio_params[0] = (void*)&mp3;                  // pointer to MP3
    audio_params[1] = (void*)(uintptr_t)0x06;       //dir name
    audio_params[2] = (void*)(uintptr_t)0x03;

    static void* vibration_params[2];
    vibration_params[0] = (void*)&vibration_motor;          
    vibration_params[1] = (void*)vibration_pattern.c_str();       
      
    if(system_settings.getMode() == "Vibration") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, vibration_params, 1, nullptr);
        vTaskDelay(1500);
    }
    if(system_settings.getMode() == "Sound") {
        //file name
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1500);
    }
    if(system_settings.getMode() == "Both") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, vibration_params, 1, nullptr);  
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1500);
    }
}

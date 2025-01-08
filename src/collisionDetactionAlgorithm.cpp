#include <Arduino.h>
#include <math.h>
#include "collisionDetectionAlgorithm.h"

// If the absolute velocity is below this threshold, we’ll consider it “stopped”
static const double VELOCITY_THRESHOLD  = 0.1; // in m/s
static const float ACC_THRESHOLD  = 0.4f;  // in m/s
static const float G = 9.81f; // Gravity in m/s^2
static float horizonAccXBuffer[FILTER_SIZE] = {0.0};

// Calculate velocity using the accelerometer data
void calculateVelocity(const SensorData& sensorData, double *velocity, float deltaTime) {
    static float prevAccelX = 0.0f; // Store the previous acceleration sample
    float accelX = sensorData.getLinearAccelX(); // Current acceleration in m/s^2

    // Calculate change in acceleration
    float deltaAccX = accelX; //- prevAccelX;
    prevAccelX = accelX; // Update the previous acceleration

    // Ignore insignificant changes to filter noise
    if (fabs(deltaAccX) < ACC_THRESHOLD) {
        Serial.println("Ignoring insignificant change in acceleration");
        deltaAccX = 0.0f;
    }

    // Integrate the change in acceleration over time to calculate velocity
    *velocity += (double(deltaAccX) * deltaTime / 1000.0);

    // Apply threshold to set velocity to zero if below a certain value
    if (*velocity < VELOCITY_THRESHOLD) {
        *velocity = 0.0;
    }

    // Debug output
    Serial.println();
    Serial.print("Current AccX: ");
    Serial.print(accelX);
    Serial.print(" | Delta AccX: ");
    Serial.print(deltaAccX);
    Serial.print(" | Velocity: ");
    Serial.println(*velocity * G);
}

// Calculate velocity using the accelerometer data with Zero Velocity Update (ZUPT)
/*void calculateVelocityWithZUPT(const SensorData& sensorData, double* velocity, float deltaTime) {
    static float prevAccelX = 0.0f;
    static float prevGyroX = 0.0f;
    static float accelBuffer[FILTER_SIZE] = {0.0};
    static float gyroBuffer[FILTER_SIZE] = {0.0};
    static int filterIndex = 0;
    static bool isStationary = false;

    // Current sensor readings
    float rawAccelX = sensorData.getLinearAccelX();  // Raw X-axis acceleration
    float rawGyroX = sensorData.getGyroX();          // Raw X-axis angular velocity

    // Apply smoothing
    float accelX = applySmoothing(rawAccelX, accelBuffer);
    float gyroX = applySmoothing(rawGyroX, gyroBuffer);

    // Calculate deltas
    //float deltaAccX = accelX - prevAccelX;
    float deltaAccX = accelX;
    float deltaGyroX = gyroX; // - prevGyroX;

    // Calculate the magnitude of acceleration and gyroscope deltas
    float accelMagnitude = fabs(deltaAccX);
    float gyroMagnitude = fabs(deltaGyroX);

    // Thresholds to detect stationary periods
    const float ACC_THRESHOLD = 0.1;   // Increased threshold
    const float GYRO_THRESHOLD = 0.1;
    const float ACC_HYSTERESIS = 0.05;
    const float GYRO_HYSTERESIS = 0.02;

    // Detect stationary state with hysteresis 
    if (accelMagnitude < (ACC_THRESHOLD - ACC_HYSTERESIS) && 
        gyroMagnitude < (GYRO_THRESHOLD - GYRO_HYSTERESIS)) {
        isStationary = true;
    } else if (accelMagnitude > (ACC_THRESHOLD + ACC_HYSTERESIS) || 
               gyroMagnitude > (GYRO_THRESHOLD + GYRO_HYSTERESIS)) {
        isStationary = false;
    }

    // Apply ZUPT
    if (isStationary) {
        *velocity = 0.0;  // Reset velocity
        Serial.println("Stationary: Velocity reset to 0.");
    } else {
        // Integrate delta acceleration to update velocity
        *velocity += deltaAccX * deltaTime / 1000.0;

        // Ensure velocity doesn't grow unreasonably (optional damping)
        if (fabs(*velocity) < VELOCITY_THRESHOLD || *velocity < 0) {
            *velocity = 0.0;
        }
    }

    // Debug output
    Serial.println();
    Serial.print("Smoothed AccX: ");
    Serial.print(accelX);
    Serial.print(" | Delta AccX: ");
    Serial.print(deltaAccX);
    Serial.print(" | Velocity: ");
    Serial.println(*velocity * 9.81); // Convert velocity to m/s² equivalent

    // Update previous values
    prevAccelX = accelX;
    prevGyroX = gyroX;
}
*/
void calculateVelocityWithZUPT(const SensorData& sensorData, double* velocity, float deltaTime) {
    
    static bool isStationary = false;

    // Apply smoothing to the raw data
    float smoothedAccX = sensorData.getLinearAccelX(); //applySmoothing(rawAccX, accelBufferX);
    float smoothedAccY = sensorData.getLinearAccelY(); //applySmoothing(rawAccY, accelBufferY);
    float smoothedAccZ = sensorData.getLinearAccelZ(); //applySmoothing(rawAccZ, accelBufferZ);
    float smoothedGyroX = sensorData.getGyroX(); //applySmoothing(rawGyroX, gyroBufferX);
    float smoothedGyroY = sensorData.getGyroY(); //applySmoothing(rawGyroY, gyroBufferY);
    float smoothedGyroZ = sensorData.getGyroZ(); //applySmoothing(rawGyroZ, gyroBufferZ);

    // Calculate total acceleration and gyroscope magnitudes
    float totalAcc = sqrt(smoothedAccX * smoothedAccX +
                          smoothedAccY * smoothedAccY +
                          smoothedAccZ * smoothedAccZ);

    float totalGyro = sqrt(smoothedGyroX * smoothedGyroX +
                           smoothedGyroY * smoothedGyroY +
                           smoothedGyroZ * smoothedGyroZ);

    // Calculate the projected X acceleration
    float effectiveAccX = (totalAcc > 0) ? (smoothedAccX / totalAcc) * totalAcc : 0.0f;

    // Thresholds for detecting stationary state
    const float ACC_THRESHOLD = 0.1;      // Threshold for acceleration magnitude
    const float GYRO_THRESHOLD = 0.1;    // Threshold for gyroscope magnitude
    const float ACC_HYSTERESIS = 0.02;    // Hysteresis for acceleration
    const float GYRO_HYSTERESIS = 0.02;   // Hysteresis for gyroscope

    // Detect stationary state using both accelerometer and gyroscope
    if (fabs(effectiveAccX) < (ACC_THRESHOLD - ACC_HYSTERESIS) &&
        totalGyro < (GYRO_THRESHOLD - GYRO_HYSTERESIS)) {
        isStationary = true;
    } else if (fabs(effectiveAccX) > (ACC_THRESHOLD + ACC_HYSTERESIS) ||
               totalGyro > (GYRO_THRESHOLD + GYRO_HYSTERESIS)) {
        isStationary = false;
    }

    // Apply ZUPT
    if (isStationary) {
        *velocity = 0.0;  // Reset velocity
        Serial.println("Stationary: Velocity reset to 0.");
    } else {
        // Integrate acceleration to update velocity
        *velocity += effectiveAccX * deltaTime / 1000.0;

        // Ensure velocity doesn't grow unreasonably (optional damping)
        if (fabs(*velocity) < 1e-5 || *velocity < 0) {
            *velocity = 0.0;
        }
    }

    // Debug output
    //Serial.println();
    //Serial.print("Smoothed AccX: ");
    //Serial.print(smoothedAccX);
    //Serial.print(" | Smoothed AccY: ");
    //Serial.print(smoothedAccY);
    //Serial.print(" | Smoothed AccZ: ");
    //Serial.print(smoothedAccZ);
    //Serial.print(" | Effective AccX: ");
    //Serial.print(effectiveAccX);
    Serial.print(" | Velocity: ");
    Serial.println(*velocity * 9.81);  // Convert velocity to m/s² equivalent
}

void calculateHorizonVelocityWithZUPT(const SensorData& sensorData, double* velocity, float deltaTime) {
    static bool isStationary = false;

    // Current raw accelerations
    float rawAccX = sensorData.getLinearAccelX();  // X-axis acceleration
    float rawAccY = sensorData.getLinearAccelY();  // Y-axis acceleration
    float rawAccZ = sensorData.getLinearAccelZ();  // Z-axis acceleration
    float rawGyroX = sensorData.getGyroX();        // X-axis angular velocity
    float rawGyroY = sensorData.getGyroY();        // Y-axis angular velocity
    float rawGyroZ = sensorData.getGyroZ();        // Z-axis angular velocity

    // Apply smoothing to the raw data
    float smoothedAccX = sensorData.getLinearAccelX(); //applySmoothing(rawAccX, accelBufferX);
    float smoothedAccY = sensorData.getLinearAccelY(); //applySmoothing(rawAccY, accelBufferY);
    float smoothedAccZ = sensorData.getLinearAccelZ(); //applySmoothing(rawAccZ, accelBufferZ);
    float smoothedGyroX = sensorData.getGyroX(); //applySmoothing(rawGyroX, gyroBufferX);
    float smoothedGyroY = sensorData.getGyroY(); //applySmoothing(rawGyroY, gyroBufferY);
    float smoothedGyroZ = sensorData.getGyroZ(); //applySmoothing(rawGyroZ, gyroBufferZ);

    // Get orientation angles (in degrees) and convert to radians
    float roll = sensorData.getRoll() * M_PI / 180.0;   // Roll angle in radians
    float pitch = sensorData.getPitch() * M_PI / 180.0; // Pitch angle in radians

    // Transform accelerations to the global (horizon-level) frame using angles in radians
    float horizonAccX = smoothedAccX * cos(pitch); //+ smoothedAccZ * sin(pitch);

    // Thresholds for detecting stationary state
    const float ACC_THRESHOLD = 0.03;      // Threshold for acceleration magnitude
    const float GYRO_THRESHOLD = 0.5;     // Threshold for gyroscope magnitude
    const float ACC_HYSTERESIS = 0.01;    // Hysteresis for acceleration
    const float GYRO_HYSTERESIS = 0.01;   // Hysteresis for gyroscope

    // Detect stationary state using both accelerometer and gyroscope
    if (fabs(horizonAccX) < (ACC_THRESHOLD - ACC_HYSTERESIS) &&
        (smoothedGyroX < (GYRO_THRESHOLD - GYRO_HYSTERESIS))) {
        isStationary = true;
    } else if (fabs(horizonAccX) > (ACC_THRESHOLD + ACC_HYSTERESIS) ||
               smoothedGyroX > (GYRO_THRESHOLD + GYRO_HYSTERESIS)) {
        isStationary = false;
    }

    // Apply ZUPT
    if (isStationary) {
        *velocity = 0.0;  // Reset velocity
        Serial.println("Stationary: Velocity reset to 0.");
    } else {
        // Integrate horizon-level acceleration to update velocity
        *velocity += horizonAccX * deltaTime / 1000.0;

        // Ensure velocity doesn't grow unreasonably (optional damping)
        if (fabs(*velocity) < 1e-5 || *velocity < 0) {
            *velocity = 0.0;
        }
    }

    // Debug output
    Serial.println();
    Serial.print("Horizon AccX: ");
    Serial.print(horizonAccX);
    Serial.print(" | Velocity: ");
    Serial.println(*velocity * 9.81);  // Convert velocity to m/s² equivalent
    Serial.print("smoothedGyroX: ");
    Serial.print(smoothedGyroX);
    Serial.print(" | smoothedGyroY: ");
    Serial.print(smoothedGyroY);
    Serial.print(" | smoothedGyroZ: ");
    Serial.println(smoothedGyroZ);
}

void calculateHorizonVelocityWithZUPT2(const SensorData& sensorData, double* velocity, float deltaTime) {
    static bool isStationary = false;
    static unsigned long stationaryStartTime = 0; // Tracks when stationary state began
    const unsigned long STATIONARY_TIME_THRESHOLD = 1000; // 1 second in milliseconds

    // Apply smoothing to the raw data
    float smoothedAccX = sensorData.getLinearAccelX(); //applySmoothing(rawAccX, accelBufferX);
    float smoothedAccY = sensorData.getLinearAccelY(); //applySmoothing(rawAccY, accelBufferY);
    float smoothedAccZ = sensorData.getLinearAccelZ(); //applySmoothing(rawAccZ, accelBufferZ);
    float smoothedGyroX = sensorData.getGyroX(); //applySmoothing(rawGyroX, gyroBufferX);
    float smoothedGyroY = sensorData.getGyroY(); //applySmoothing(rawGyroY, gyroBufferY);
    float smoothedGyroZ = sensorData.getGyroZ(); //applySmoothing(rawGyroZ, gyroBufferZ);

    // Get orientation angles (in degrees) and convert to radians
    float pitch = sensorData.getPitch() * M_PI / 180.0; // Pitch angle in radians
    float roll = sensorData.getRoll() * M_PI / 180.0;   // Roll angle in radians

    // Remove gravity's effect from the accelerations
    float correctedAccX = smoothedAccX - (-9.81 * sin(pitch));
    float correctedAccY = smoothedAccY - (9.81 * cos(pitch) * sin(roll));
    float correctedAccZ = smoothedAccZ - (9.81 * cos(pitch) * cos(roll));

    // Transform accelerations to the global (horizon-level) frame using pitch angle
    float horizonAccX = smoothedAccX; //* cos(pitch) + smoothedAccZ * sin(pitch);

    // Thresholds for detecting stationary state
    const float ACC_THRESHOLD = 0.5;      // Adjusted threshold for walking detection
    const float GYRO_THRESHOLD = 0.3;    // Threshold for gyroscope magnitude
    const float ACC_HYSTERESIS = 0.2;    // Hysteresis for acceleration
    const float GYRO_HYSTERESIS = 0.2;   // Hysteresis for gyroscope

    // Check if current data indicates a stationary state
    bool currentStationaryState = (fabs(horizonAccX) < (ACC_THRESHOLD - ACC_HYSTERESIS)) ||
                                  (fabs(smoothedGyroX) < (GYRO_THRESHOLD - GYRO_HYSTERESIS));

    // Implement time-based stationary detection
    unsigned long currentTime = millis();
    if (currentStationaryState) {
        if (!isStationary) {
            if (stationaryStartTime == 0) {
                stationaryStartTime = currentTime; // Start the stationary timer
            } else if (currentTime - stationaryStartTime >= STATIONARY_TIME_THRESHOLD) {
                isStationary = true; // Confirm stationary state after threshold time
            }
        }
    } else {
        stationaryStartTime = 0; // Reset timer if no longer stationary
        isStationary = false;    // Reset stationary flag
    }

    // Apply ZUPT
    if (isStationary) {
        *velocity = 0.0;  // Reset velocity
        Serial.println("Stationary: Velocity reset to 0.");
    } else {
        // Integrate horizon-level acceleration to update velocity
        *velocity += horizonAccX * deltaTime / 1000.0;

        // Ensure velocity doesn't grow unreasonably (optional damping)
        if (fabs(*velocity) < 1e-5 || *velocity < 0) {
            *velocity = 0.0;
        }
    }

    // Debug output
    Serial.println();
    Serial.print("Horizon AccX: ");
    Serial.print(horizonAccX);
    Serial.print(" | Velocity: ");
    Serial.println(*velocity * 9.81);  // Convert velocity to m/s² equivalent
    Serial.print("Gyro X: ");
    Serial.println(smoothedGyroX);
}

/*void calculateStepCount(const SensorData& sensorData, int* stepCount) {
    static bool isStepDetected = false;
    static float prevAccX = 0.0f;       // Previous X-axis acceleration
    static float prevGyroMagnitude = 0.0f; // Previous gyroscope magnitude
    static unsigned long lastStepTime = 0; // Time of the last detected step
    const unsigned long STEP_TIME_THRESHOLD = 500; // Minimum time between steps in milliseconds (for debouncing)

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
    const float GYRO_THRESHOLD = 0.08f;        // Minimum gyroscope change to indicate a step

    // Get current time
    unsigned long currentTime = millis();

    // Step detection logic using both acceleration and gyroscope variation
    if (!isStepDetected && deltaAccX > STEP_HIGH_THRESHOLD && deltaGyroMagnitude > GYRO_THRESHOLD &&
        (currentTime - lastStepTime) > STEP_TIME_THRESHOLD) {
        // Step peak detected
        isStepDetected = true;
        *stepCount += 1; // Increment step count
        lastStepTime = currentTime;

        Serial.println("Step detected (with gyroscope)");
    } else if (isStepDetected && deltaAccX < STEP_LOW_THRESHOLD && deltaGyroMagnitude < GYRO_THRESHOLD) {
        // Reset state after step detected
        isStepDetected = false;
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
}*/

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
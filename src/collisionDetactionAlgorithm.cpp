#include <Arduino.h>
#include <math.h>
#include "collisionDetectionAlgorithm.h"

// If the absolute velocity is below this threshold, we’ll consider it “stopped”
static const double VELOCITY_THRESHOLD  = 0.02; // in m/s
static const float ACC_THRESHOLD  = 0.02f;  // in m/s
static const float G = 9.81f; // Gravity in m/s^2
#define FILTER_SIZE 10 // Size of the moving average filter
static float accelBuffer[FILTER_SIZE] = {0.0};
static float gyroBuffer[FILTER_SIZE] = {0.0};
static int filterIndex = 0;

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

// Apply a simple moving average filter to smooth out the sensor readings
float applySmoothing(float newValue, float* buffer) {
    buffer[filterIndex] = newValue;
    filterIndex = (filterIndex + 1) % FILTER_SIZE;

    float sum = 0.0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += buffer[i];
    }
    return sum / FILTER_SIZE;
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
    static float accelBufferX[FILTER_SIZE] = {0.0};
    static float accelBufferY[FILTER_SIZE] = {0.0};
    static float accelBufferZ[FILTER_SIZE] = {0.0};
    static float gyroBufferX[FILTER_SIZE] = {0.0};
    static float gyroBufferY[FILTER_SIZE] = {0.0};
    static float gyroBufferZ[FILTER_SIZE] = {0.0};
    static bool isStationary = false;

    // Current raw accelerations and angular velocities
    float rawAccX = sensorData.getLinearAccelX();  // X-axis acceleration
    float rawAccY = sensorData.getLinearAccelY();  // Y-axis acceleration
    float rawAccZ = sensorData.getLinearAccelZ();  // Z-axis acceleration
    float rawGyroX = sensorData.getGyroX();        // X-axis angular velocity
    float rawGyroY = sensorData.getGyroY();        // Y-axis angular velocity
    float rawGyroZ = sensorData.getGyroZ();        // Z-axis angular velocity

    // Apply smoothing to the raw data
    float smoothedAccX = applySmoothing(rawAccX, accelBufferX);
    float smoothedAccY = applySmoothing(rawAccY, accelBufferY);
    float smoothedAccZ = applySmoothing(rawAccZ, accelBufferZ);
    float smoothedGyroX = applySmoothing(rawGyroX, gyroBufferX);
    float smoothedGyroY = applySmoothing(rawGyroY, gyroBufferY);
    float smoothedGyroZ = applySmoothing(rawGyroZ, gyroBufferZ);

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
    const float GYRO_THRESHOLD = 0.05;    // Threshold for gyroscope magnitude
    const float ACC_HYSTERESIS = 0.05;    // Hysteresis for acceleration
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
    Serial.println();
    Serial.print("Smoothed AccX: ");
    Serial.print(smoothedAccX);
    Serial.print(" | Smoothed AccY: ");
    Serial.print(smoothedAccY);
    Serial.print(" | Smoothed AccZ: ");
    Serial.print(smoothedAccZ);
    Serial.print(" | Smoothed GyroX: ");
    Serial.print(smoothedGyroX);
    Serial.print(" | Smoothed GyroY: ");
    Serial.print(smoothedGyroY);
    Serial.print(" | Smoothed GyroZ: ");
    Serial.print(smoothedGyroZ);
    Serial.print(" | Effective AccX: ");
    Serial.print(effectiveAccX);
    Serial.print(" | Velocity: ");
    Serial.println(*velocity * 9.81);  // Convert velocity to m/s² equivalent
}
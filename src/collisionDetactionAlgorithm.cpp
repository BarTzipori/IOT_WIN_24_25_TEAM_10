#include <Arduino.h>
#include <math.h>
#include "collisionDetectionAlgorithm.h"

// If the absolute velocity is below this threshold, we’ll consider it “stopped”
static const double VELOCITY_THRESHOLD  = 0.01; // in m/s
static const float ACC_THRESHOLD  = 0.001f;  // in m/s
static const float G = 9.81f; // Gravity in m/s^2
#define FILTER_SIZE 5 // Size of the moving average filter
static float accelBuffer[FILTER_SIZE] = {0.0};
static float gyroBuffer[FILTER_SIZE] = {0.0};
static int filterIndex = 0;

// Calculate velocity using the accelerometer data
void calculateVelocity(const SensorData& sensorData, double *velocity, float deltaTime) {
    static float prevAccelX = 0.0f; // Store the previous acceleration sample
    float accelX = sensorData.getLinearAccelX(); // Current acceleration in m/s^2

    // Calculate change in acceleration
    float deltaAccX = accelX - prevAccelX;
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
void calculateVelocityWithZUPT(const SensorData& sensorData, double* velocity, float deltaTime) {
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
    float deltaAccX = accelX - prevAccelX;
    float deltaGyroX = gyroX - prevGyroX;

    // Calculate the magnitude of acceleration and gyroscope deltas
    float accelMagnitude = fabs(deltaAccX);
    float gyroMagnitude = fabs(deltaGyroX);

    // Thresholds to detect stationary periods
    const float ACC_THRESHOLD = 0.1;   // Increased threshold
    const float GYRO_THRESHOLD = 0.05;
    const float ACC_HYSTERESIS = 0.02;
    const float GYRO_HYSTERESIS = 0.01;

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
#include <Arduino.h>
#include "collisionDetectionAlgorithm.h"

// If the absolute velocity is below this threshold, we’ll consider it “stopped”
static const float VELOCITY_THRESHOLD  = 0.05f;  // in m/s
static const float ACC_THRESHOLD  = 0.02f;  // in m/s

void calculateVelocity(const SensorData& sensorData, float &velocity, float deltaTime) {
    float accelX = sensorData.getLinearAccelX(); // m/s^2
    if (fabs(accelX) < ACC_THRESHOLD) {
        accelX = 0.0f;
    }
    // Integrate acceleration to update velocity
    velocity += fabs(accelX) * deltaTime/1000;

    // Apply threshold to determine if velocity is effectively zero
    if (velocity < VELOCITY_THRESHOLD) {
        velocity = 0.0f;
    }
}

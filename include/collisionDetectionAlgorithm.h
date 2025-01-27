#ifndef COLLISION_DETECTION_ALGORITHM_H
#define COLLISION_DETECTION_ALGORITHM_H

#include <arduino.h>
#include "sensorData.h"
#include "systemSettings.h"
#include "parameters.h"
#include "vibrationMotor.h"
#include "RedMP3.h"
#include "safestep_logs.h"

struct VelocityTaskParams
{
    int delay_in_ms;          // Delay in milliseconds
    systemSettings *settings; // Pointer to systemSettings object
    double *velocity;         // Pointer to the velocity variable
    int *step_count;
    const SensorData *sensor_data;
    bool *system_on_flag;
};

// Function prototypes
void vibrateMotorsAsTask(void *pvParameters);
void playMP3AsTask(void *pvParameters);
void calculateStepCountAndSpeed(const SensorData& sensorData, int* stepCount, double* velocity, float userHeight);
double nearestObstacleCollisionTime(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity);
bool collisionTimeAlertHandler(double collision_time, systemSettings& system_settings, const MP3& mp3, vibrationMotor& motor1);
bool obstacleDistanceAlertHandler(double obstacle_distance, systemSettings& system_settings, const MP3& mp3, vibrationMotor& motor1);
double distanceToNearestObstacle(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity, bool mpu_degraded_flag);
void collisionAlert(const systemSettings& system_settings, const MP3& mp3, vibrationMotor& vibration_motor, String vib_pattern, uint alert_sound_type);
void calculateVelocityAsTask(void *pvParameters);
#endif
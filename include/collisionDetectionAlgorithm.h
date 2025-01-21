#ifndef COLLISION_DETECTION_ALGORITHM_H
#define COLLISION_DETECTION_ALGORITHM_H

#include <arduino.h>
#include "sensorData.h"
#include "systemSettings.h"
#include "parameters.h"
#include "vibrationMotor.h"
#include "RedMP3.h"

// Function prototypes
void vibrateMotorsAsTask(void *pvParameters);
void playMP3AsTask(void *pvParameters);
void calculateStepCountAndSpeed(const SensorData& sensorData, int* stepCount, double* velocity, float userHeight);
double nearestObstacleCollisionTime(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity);
bool collisionTimeAlertHandler(double collision_time, systemSettings& system_settings, const MP3& mp3, vibrationMotor& motor1);
bool obstacleDistanceAlertHandler(double obstacle_distance, systemSettings& system_settings, const MP3& mp3, vibrationMotor& motor1);
double distanceToNearestObstacle(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity, bool mpu_degraded_flag);
void collisionAlert(const systemSettings& system_settings, const MP3& mp3, vibrationMotor& vibration_motor, String vib_pattern, uint alert_sound_type);
#endif
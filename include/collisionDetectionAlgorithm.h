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
void calculateStepCountAndSpeed(const SensorData& sensorData, int* stepCount, float* velocity, float userHeight);
double collisionDetector(const SensorData& sensor_data, const systemSettings& system_settings, float* velocity);
void collisionAlert(const systemSettings& system_settings, const MP3& mp3, vibrationMotor& vibration_motor, String vibration_pattern);
#endif
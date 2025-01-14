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
double collisionDetector(const SensorData& sensor_data, const systemSettings& system_settings, double* velocity);
void collisionAlert(const systemSettings& system_settings, const MP3& mp3, vibrationMotor& vibration_motor, String vib_pattern, uint alert_sound_type);
#endif
#ifndef COLLISION_DETECTION_ALGORITHM_H
#define COLLISION_DETECTION_ALGORITHM_H

#include <arduino.h>
#include "sensorData.h"

// Function prototypes
void calculateVelocity(const SensorData& sensorData, double *velocity, float deltaTime);
void calculateVelocityWithZUPT(const SensorData& sensorData, double* velocity, float deltaTime);
void calculateHorizonVelocityWithZUPT(const SensorData& sensorData, double* velocity, float deltaTime);
void calculateHorizonVelocityWithZUPT2(const SensorData& sensorData, double* velocity, float deltaTime);
#endif
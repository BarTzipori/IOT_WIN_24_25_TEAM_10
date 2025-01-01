#ifndef COLLISION_DETECTION_ALGORITHM_H
#define COLLISION_DETECTION_ALGORITHM_H

#include <arduino.h>
#include "sensorData.h"

// Function prototypes
void calculateVelocity(const SensorData& sensorData, float &velocity, float deltaTime);

#endif
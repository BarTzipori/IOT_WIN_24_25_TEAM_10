#ifndef SENSOR_HELPER_FUNCTIONS_H
#define SENSOR_HELPER_FUNCTIONS_H

#include <Arduino.h>
#include "Adafruit_VL53L1X.h"
#include "MPU9250.h"
#include <vector>
#include <utility>

void printMPURollPitchYaw(MPU9250* mpu);
void printMPUCalibration(MPU9250* mpu);
void printMPUSensorData(MPU9250* mpu);
void calibrateMPU(MPU9250& mpu, bool calibration_needed);
bool isVL53L1XSensorConnected(uint8_t address);
bool initializeVL53L1XSensor(Adafruit_VL53L1X* sensor, int xshut_pin, int i2c_address);
void disableAllVL53L1XSensors(std::vector<int>* distance_sensors_xshut_pins);
void enableAllVL53L1XSensors(std::vector<int>* distance_sensors_xshut_pins);

#endif
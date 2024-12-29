#include "sensorData.h"

void SensorData::setPitch(float pitchValue) {
    this->pitch = pitchValue;
}
void SensorData::setYaw(float yawValue) {
    this->yaw = yawValue;
}
void SensorData::setRoll(float rollValue) {
    this->roll = rollValue;
}
void SensorData::setAccelX(float accelXValue) {
    this->accelX = accelXValue;
}
void SensorData::setAccelY(float accelYValue) {
    this->accelY = accelYValue;
}
void SensorData::setAccelZ(float accelZValue) {
    this->accelZ = accelZValue;
}
void SensorData::setSensor1Distance(int distance) {
    this->sensor1distance = distance;
}
void SensorData::setSensor2Distance(int distance) {
    this->sensor2distance = distance;
}

void SensorData::setlastUpdateTime(uint32_t distance) {
    this->lastUpdateTime = distance;
}
void SensorData::updateLinearAccelX() {
    this->linearAccelX = fabs(accelZ)*cos(90-this->pitch);
}

void SensorData::printData() const {
    Serial.print("MPU Data -> Pitch: ");
    Serial.print(this->pitch,2);
    Serial.print(", Roll: ");
    Serial.print(this->roll,2);
    Serial.print(", Yaw: ");
    Serial.print(this->yaw,2);
    Serial.print(", AccelX: ");
    Serial.print(this->accelX,2);
    Serial.print(", LinearAccelX: ");
    Serial.print(this->linearAccelX,2);
    Serial.print(", AccelY: ");
    Serial.print(this->accelY,2);
    Serial.print(", AccelZ: ");
    Serial.println(this->accelZ,2);
    Serial.print("Distance Sensors -> Sensor1: ");
    Serial.print(this->sensor1distance);
    Serial.print(" mm, Sensor2: ");
    Serial.print(this->sensor2distance);
    Serial.print(" mm");
    Serial.print(" Last update time: ");
    Serial.println(this->lastUpdateTime);
    Serial.println();
}
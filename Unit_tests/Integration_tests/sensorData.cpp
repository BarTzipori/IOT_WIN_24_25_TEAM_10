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
void SensorData::printData() const {
    Serial.print("MPU Data -> Pitch: ");
    Serial.print(this->pitch);
    Serial.print(", Yaw: ");
    Serial.print(this->yaw);
    Serial.print(", Roll: ");
    Serial.print(this->roll);
    Serial.print(", AccelX: ");
    Serial.print(this->accelX);
    Serial.print(", AccelY: ");
    Serial.print(this->accelY);
    Serial.print(", AccelZ: ");
    Serial.println(this->accelZ);
    Serial.print("Distance Sensors -> Sensor1: ");
    Serial.print(this->sensor1distance);
    Serial.print(" mm, Sensor2: ");
    Serial.print(this->sensor2distance);
    Serial.println(" mm");
}
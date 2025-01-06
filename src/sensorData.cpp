#include "sensorData.h"

SensorData::SensorData() : pitch(0.0), yaw(0.0), roll(0.0),
                           accelX(0.0), accelY(0.0), accelZ(0.0),
                           linearAccelX(0.0), linearAccelY(0.0), linearAccelZ(0.0),
                           gyroX(0.0), gyroY(0.0), gyroZ(0.0),
                           sensor1distance(0), sensor2distance(0), sensor3distance(0), sensor4distance(0),
                           lastUpdateTime(0), filterIndex(0) {
    // Initialize sliding window buffers to 0.0
    for (int i = 0; i < FILTER_SIZE; i++) {
        accelXBuffer[i] = 0.0f;
        accelYBuffer[i] = 0.0f;
        accelZBuffer[i] = 0.0f;
        gyroXBuffer[i] = 0.0f;
        gyroYBuffer[i] = 0.0f;
        gyroZBuffer[i] = 0.0f;
        pitchBuffer[i] = 0.0f;
        rollBuffer[i] = 0.0f;   
        yawBuffer[i] = 0.0f;
    }
}
void SensorData::setPitch(float pitchValue) {
    this->pitch = applySmoothing(pitchValue, pitchBuffer);
}
void SensorData::setYaw(float yawValue) {
    this->yaw = applySmoothing(yawValue, yawBuffer);
}
void SensorData::setRoll(float rollValue) {
    this->roll = applySmoothing(rollValue, rollBuffer);
}
void SensorData::setLinearAccelX(float linearAccelXValue) {
    linearAccelX = applySmoothing(linearAccelXValue, accelXBuffer);
}
void SensorData::setLinearAccelY(float linearAccelYValue) {
    linearAccelY = applySmoothing(linearAccelYValue, accelYBuffer);
}
void SensorData::setLinearAccelZ(float linearAccelZValue) {
    linearAccelZ = applySmoothing(linearAccelZValue, accelZBuffer);
}
void SensorData::setGyroX(float gyroXValue) {
    gyroX = applySmoothing(gyroXValue, gyroXBuffer);
}
void SensorData::setGyroY(float gyroYValue) {
    gyroY = applySmoothing(gyroYValue, gyroYBuffer);
}
void SensorData::setGyroZ(float gyroZValue) {
    gyroZ = applySmoothing(gyroZValue, gyroZBuffer);
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

void SensorData::setSensor3Distance(int distance) {
    this->sensor3distance = distance;
}
void SensorData::setSensor4Distance(int distance) {
    this->sensor4distance = distance;
}
void SensorData::setlastUpdateTime(uint32_t distance) {
    this->lastUpdateTime = distance;
}
void SensorData::updateLinearAccelX() {
    linearAccelX = accelX - (accelZ)*(sin(pitch* M_PI / 180.0));
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
    Serial.print(this->accelZ,2);
    Serial.print(", GyroX: ");
    Serial.println(this->gyroX,2);
    Serial.print("Distance Sensors -> Sensor1: ");
    Serial.print(this->sensor1distance);
    Serial.print(" mm, Sensor2: ");
    Serial.print(this->sensor2distance);
    Serial.print(" mm, Sensor3: ");
    Serial.print(this->sensor3distance);
    Serial.print(" mm, Sensor4: ");
    Serial.print(this->sensor4distance);
    Serial.println(" mm");
    Serial.print(" Last update time: ");
    Serial.println(this->lastUpdateTime);
    Serial.println();
}

// Apply sliding window smoothing
float SensorData::applySmoothing(float newValue, float* buffer) {
    buffer[filterIndex] = newValue;
    filterIndex = (filterIndex + 1) % FILTER_SIZE;

    float sum = 0.0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += buffer[i];
    }
    return sum / FILTER_SIZE;
}
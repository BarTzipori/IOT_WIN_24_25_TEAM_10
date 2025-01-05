#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <Arduino.h>

class SensorData {
private:

    float pitch;
    float yaw;
    float roll;
    float accelX;
    float accelY;
    float accelZ;
    float linearAccelX;
    float linearAccelY;
    float linearAccelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    int sensor1distance;
    int sensor2distance;
    int sensor3distance;
    int sensor4distance;
    uint32_t lastUpdateTime; // Holds the timestamp in ms or µs

public:
    // Constructor
    SensorData(): pitch(0.0), yaw(0.0), roll(0.0), accelX(0.0), accelY(0.0), accelZ(0.0), linearAccelX(0.0), linearAccelY(0.0), linearAccelZ(0.0), gyroX(0.0), gyroY(0.0), gyroZ(0.0), sensor1distance(0), sensor2distance(0), sensor3distance(0), sensor4distance(0), lastUpdateTime(0) {}

    void setPitch(float pitchValue);
    void setYaw(float yawValue);
    void setRoll(float rollValue);
    void setAccelX(float accelXValue);
    void setAccelY(float accelYValue);
    void setAccelZ(float accelZValue);
    void setSensor1Distance(int distance);
    void setSensor2Distance(int distance);
    void setSensor3Distance(int distance);
    void setSensor4Distance(int distance);
    void setGyroX(float gyroXValue);
    void setGyroY(float gyroYValue);
    void setGyroZ(float gyroZValue);
    void setlastUpdateTime(uint32_t time);


    float getPitch() const { return pitch; }
    float getYaw() const { return yaw; }
    float getRoll() const { return roll; }
    float getAccelX() const { return accelX; }
    float getAccelY() const { return accelY; }
    float getAccelZ() const { return accelZ; }
    float getLinearAccelX() const { return linearAccelX; }
    float getLinearAccelY() const { return linearAccelY; }
    float getLinearAccelZ() const { return linearAccelZ; }
    float getGyroX() const { return gyroX; }
    float getGyroY() const { return gyroY; }
    float getGyroZ() const { return gyroZ; }
    void updateLinearAccelX();

    int getDistanceSensor1() const { return sensor1distance; }
    int getDistanceSensor2() const { return sensor2distance; }
    int getDistanceSensor3() const { return sensor3distance; }
    int getDistanceSensor4() const { return sensor4distance; }
    uint32_t getlastUpdateTime() const { return lastUpdateTime; }

    // Print all data (for debugging or logging)
    void printData() const;
};

#endif
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
    int sensor1distance;
    int sensor2distance;

public:
    // Constructor
    SensorData(): pitch(0.0), yaw(0.0), roll(0.0), accelX(0.0), accelY(0.0), accelZ(0.0), sensor1distance(0), sensor2distance(0) {}

    void setPitch(float pitchValue);
    void setYaw(float yawValue);
    void setRoll(float rollValue);
    void setAccelX(float accelXValue);
    void setAccelY(float accelYValue);
    void setAccelZ(float accelZValue);
    void setSensor1Distance(int distance);
    void setSensor2Distance(int distance);

    float getPitch() const { return pitch; }
    float getYaw() const { return yaw; }
    float getRoll() const { return roll; }
    float getAccelX() const { return accelX; }
    float getAccelY() const { return accelY; }
    float getAccelZ() const { return accelZ; }

    int getDistanceSensor1() const { return sensor1distance; }
    int getDistanceSensor2() const { return sensor2distance; }

    // Print all data (for debugging or logging)
    void printData() const;
};

#endif
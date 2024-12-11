#include "MPU9250.h"
#include <Arduino.h>

MPU9250 mpu;
double yaw_offset=0;
void print_roll_pitch_yaw(float offset)
{
  Serial.print("Yaw, Pitch, Roll: ");
  Serial.print(mpu.getYaw()-offset, 2);
  Serial.print(", ");
  Serial.print(mpu.getPitch(), 2);
  Serial.print(", ");
  Serial.println(mpu.getRoll(), 2);
  Serial.print(",offset ");
  Serial.println(offset, 2);
}
void print_calibration() {
    Serial.println("< calibration parameters >");
    Serial.println("accel bias [g]: ");
    Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println();
    Serial.println("gyro bias [deg/s]: ");
    Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println();
    Serial.println("mag bias [mG]: ");
    Serial.print(mpu.getMagBiasX());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasY());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasZ());
    Serial.println();
    Serial.println("mag scale []: ");
    Serial.print(mpu.getMagScaleX());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleY());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleZ());
    Serial.println();
}
void printSensorData() {
      Serial.println("accel x: ");
      Serial.println(mpu.getAccX());
      Serial.println(mpu.getLinearAccX());

      Serial.println("accel y: ");
      Serial.println(mpu.getAccY());
      Serial.println(mpu.getLinearAccY());
      Serial.println("accel z: ");
      Serial.println(mpu.getAccZ());
      Serial.println(mpu.getLinearAccZ());
}

void setup() {
    Serial.begin(115200);
    Wire.begin(18, 17);
    delay(2000);

    if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }

    // calibrate anytime you want to
    Serial.println("Accel Gyro calibration will start in 5sec.");
    Serial.println("Please leave the device still on the flat plane.");
    mpu.verbose(true);
    delay(5000);
    mpu.calibrateAccelGyro();

    Serial.println("Mag calibration will start in 5sec.");
    Serial.println("Please Wave device in a figure eight until done.");
    delay(5000);
    mpu.calibrateMag();
    Serial.println("done calibrating");


    print_calibration();
    mpu.verbose(true);
    mpu.setMagneticDeclination(5.14);
    mpu.setFilterIterations(10);
    mpu.selectFilter(QuatFilterSel::MADGWICK);

    
}
int indx = 0;
void loop()
{
  indx++;
  if (mpu.update())
  {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 25)
    {
      print_roll_pitch_yaw(yaw_offset);
      prev_ms = millis();
    }
    if(indx==1000){
      Serial.println("put systen in wanted direction");
      delay(5000);
      yaw_offset=mpu.getYaw();
      Serial.print(",offset ");
      Serial.println(yaw_offset, 2);
      delay(5000);
    }
    // printSensorData();
  }
}

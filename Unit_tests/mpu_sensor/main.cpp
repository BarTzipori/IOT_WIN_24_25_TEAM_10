#include "MPU9250.h"
#include "eeprom_utils.h"
#include <Arduino.h>

MPU9250 mpu;
double yaw_offset=0;
bool calibration_needed = false;

void print_roll_pitch_yaw()
{
  Serial.print("Yaw, Pitch, Roll: ");
  float yaw = mpu.getYaw();
  if (yaw < 0)
  {
    yaw += 360;
  }
  Serial.print(yaw, 2);
  Serial.print(", ");
  Serial.print(mpu.getPitch(), 2);
  Serial.print(", ");
  Serial.println(mpu.getRoll(), 2);

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
    #if defined(ESP_PLATFORM) || defined(ESP8266)
        EEPROM.begin(0x80);
    #endif

    delay(5000);
    if(calibration_needed) {
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
      // save to eeprom
      saveCalibration();
    } 
    // load from eeprom
    loadCalibration();

    print_calibration();
    mpu.ahrs(true);
    mpu.setMagneticDeclination(5.14);
    mpu.setFilterIterations(10);
    mpu.selectFilter(QuatFilterSel::MADGWICK);
}

void loop()
{
  if (mpu.update())
  {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 25)
    {
      print_roll_pitch_yaw();
      prev_ms = millis();
    }
  }
}

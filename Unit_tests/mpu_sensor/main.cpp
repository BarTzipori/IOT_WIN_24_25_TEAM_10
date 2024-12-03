#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire, 0x68);
int status;

void setup() {
  // serial to display data
  Serial.begin(115200);
  Wire.begin(18,17);
  while (!Serial) delay(10);

  // start communication with IMU
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1) {}
  }
}

void loop() {
  // read the sensor
  IMU.readSensor();
  // display the data
  Serial.print("AccelX: ");
  Serial.println(IMU.getAccelX_mss(), 6);
  Serial.print("accelY: ");
  Serial.println(IMU.getAccelY_mss(), 6);
  Serial.print("accelZ: ");
  Serial.println(IMU.getAccelZ_mss(), 6);
  Serial.print("GyroX: ");
  Serial.println(IMU.getGyroX_rads(), 6);
  Serial.print("GyroY: ");
  Serial.println(IMU.getGyroY_rads(), 6);
  Serial.print("GyroZ: ");  
  Serial.println(IMU.getGyroZ_rads(), 6);
  Serial.print("MagX: ");
  Serial.println(IMU.getMagX_uT(), 6);
  Serial.print("MagY: ");
  Serial.println(IMU.getMagY_uT(), 6);
  Serial.print("MagZ: ");
  Serial.println(IMU.getMagZ_uT(), 6);
  Serial.print("Temperature: ");
  Serial.println(IMU.getTemperature_C(), 6);
  Serial.println();
  delay(200);
  
}
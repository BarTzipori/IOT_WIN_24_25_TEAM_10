#include "sensorHelperFunctions.h"
#include "eeprom_utils.h"

void printMPURollPitchYaw(MPU9250* mpu){
    float yaw = mpu->getYaw();
    if(yaw < 0 ) {
        yaw = 360 + yaw;
    }
    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(yaw, 2);
    Serial.print(", ");
    Serial.print(mpu->getPitch(), 2);
    Serial.print(", ");
    Serial.println(mpu->getRoll(), 2);
}

void printMPUCalibration(MPU9250* mpu) {
    Serial.println("< calibration parameters >");
    Serial.println("accel bias [g]: ");
    Serial.print(mpu->getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu->getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.println(mpu->getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println("gyro bias [deg/s]: ");
    Serial.print(mpu->getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu->getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.println(mpu->getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println("mag bias [mG]: ");
    Serial.print(mpu->getMagBiasX());
    Serial.print(", ");
    Serial.print(mpu->getMagBiasY());
    Serial.print(", ");
    Serial.println(mpu->getMagBiasZ());
    Serial.println("mag scale []: ");
    Serial.print(mpu->getMagScaleX());
    Serial.print(", ");
    Serial.print(mpu->getMagScaleY());
    Serial.print(", ");
    Serial.println(mpu->getMagScaleZ());
}

void printMPUSensorData(MPU9250* mpu) {
    Serial.println("accel x: ");
    Serial.println(mpu->getAccX());
    Serial.println(mpu->getLinearAccX());
    Serial.println("accel y: ");
    Serial.println(mpu->getAccY());
    Serial.println(mpu->getLinearAccY());
    Serial.println("accel z: ");
    Serial.println(mpu->getAccZ());
    Serial.println(mpu->getLinearAccZ());
}

// Helper function to check if sensor is connected
bool isVL53L1XSensorConnected(uint8_t address, TwoWire* wire) {
    wire->beginTransmission(address);
    return (wire->endTransmission() == 0); // Returns true if sensor is responsive
}

bool initializeVL53L1XSensor(Adafruit_VL53L1X* sensor, int xshut_pin, int i2c_address, TwoWire* sensor_wire) {
    pinMode(xshut_pin, OUTPUT);
    digitalWrite(xshut_pin, LOW);
    delay(100);
    bool init_flag = sensor->begin(i2c_address, sensor_wire);
    if(init_flag ) {
        sensor->setTimingBudget(50000);
        sensor->startRanging();
        return true;
    } else {
        Serial.println("Failed to initialize VL53L1X sensor");
        return false;
    }
}

void disableAllVL53L1XSensors(std::vector<int>* distance_sensors_xshut_pins) {
    for (size_t i = 0; i < distance_sensors_xshut_pins->size(); i++) {
        digitalWrite(distance_sensors_xshut_pins->at(i), LOW);
    }
}

void enableAllVL53L1XSensors(std::vector<int>* distance_sensors_xshut_pins) {
    for (size_t i = 0; i < distance_sensors_xshut_pins->size(); i++) {
        digitalWrite(distance_sensors_xshut_pins->at(i), HIGH);
    }
}

void calibrateMPU(MPU9250* mpu, bool calibration_needed) {
    #if defined(ESP_PLATFORM) || defined(ESP8266)
        EEPROM.begin(0x80);
    #endif
    if(calibration_needed) {
      // calibrate anytime you want to
      Serial.println("Accel Gyro calibration will start in 5sec.");
      Serial.println("Please leave the device still on the flat plane.");
      mpu->verbose(true);
      delay(5000);
      mpu->calibrateAccelGyro();

      Serial.println("Mag calibration will start in 5sec.");
      Serial.println("Please Wave device in a figure eight until done.");
      delay(5000);
      mpu->calibrateMag();
      Serial.println("done calibrating");
      // save to eeprom
      saveCalibration();
      Serial.println("MPU calibration saved");
    } 
    // load from eeprom
    loadCalibration();
    delay(5000);
    Serial.println("MPU calibration loaded");
    printMPUCalibration(mpu);
    mpu->ahrs(true);
    mpu->setMagneticDeclination(5.14);
    mpu->setFilterIterations(20);
    mpu->selectFilter(QuatFilterSel::MADGWICK);
}


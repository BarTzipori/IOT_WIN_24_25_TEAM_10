#include "MPU9250.h"
#include "Adafruit_VL53L1X.h"
#include <vector>
#include <utility>
#include <Arduino.h>

#define IRQ_PIN 2
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 5

Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2};

std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, 0x30},  {&vl53_2, 0x31}};

MPU9250 mpu;
double yaw_offset=0;

// Helper function to check if sensor is connected
bool isSensorConnected(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0); // Returns true if sensor is responsive
}

// Helper function to initialize a sensor with a unique address
bool initializeSensor(Adafruit_VL53L1X* sensor, int xshut_pin, int i2c_address) {
    // Enable the desired sensor
    digitalWrite(xshut_pin, HIGH); // Pull XSHUT HIGH to enable
    delay(10); // Allow stabilization
    Serial.print("Sensor: ");
    Serial.print(xshut_pin);
    Serial.println(" connected, initializing."); 
    // initialize the sensor
    if (!sensor->begin(i2c_address, &Wire)) {
        Serial.printf("Error reinitializing sensor %d\n", index);
        return false;
    }
    // Start ranging 
    if (!sensor->startRanging()) {
        Serial.printf("Error starting ranging on sensor %d\n", index);
        return false;
    }
    sensor->setTimingBudget(50); //Set timing budget
    return true;
}

// Helper function to disable all sensors
void disableAllSensors() {
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], LOW);
    }
}

// Helper function to enable all sensors
void enableAllSensors() {
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], HIGH);
    }
}

// Helper function to print sensor data
void printSensorsData(int delay_in_ms) {
  for (int i = 0; i < distance_sensors.size(); i++) {
    //delay(100);
    if(!isSensorConnected(distance_sensors[i].second)) {
        Serial.print("Sensor: ");
        Serial.print(i);
        Serial.println(" not connected");
        continue;
    } else {
        while(!distance_sensors[i].first->dataReady()) {
            
        } 

        int distance = distance_sensors[i].first->distance();
            if (distance == -1) {
              Serial.print(F("Couldn't get distance: "));
              Serial.println(distance_sensors[i].first->vl_status);
              continue;
            } 
            Serial.print(F("Distance: "));
            Serial.print(distance);
            Serial.print(" mm from sensor: ");
            Serial.println(i);
            distance_sensors[i].first->clearInterrupt();
    }
  }
  Serial.println();
  delay(delay_in_ms);
}

/************************************************************* */
// mpu sensor functions:

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
 /******************************************************************* */

void setup() {
  Serial.begin(115200);
  Wire.begin(18,17);
  while (!Serial) delay(10);

  if (!mpu.setup(0x68))
  { // change to your own address
    while (1)
    {
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




  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }

  initializeSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second);
  initializeSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second);  
  
}


int indx = 0;

void loop() {
  //printSensorsData(100);
  indx++;
  //disableAllSensors();
  if (mpu.update())
  {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 25)
    {
      print_roll_pitch_yaw(yaw_offset);
      //enableAllSensors();
     // delay(10);
      
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

    printSensorsData(10);
   // printSensorData();
  }


}

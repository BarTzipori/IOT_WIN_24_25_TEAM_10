
#include <VL53L1X.h>

void setup() {
    // Initialize serial communication at 115200 baud
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    // Give some time for the Serial Monitor to initialize
    delay(1000);

    // Print a message to the Serial Monitor
    Serial.println("Serial communication started!");
}

void loop() {
    // Print a message repeatedly
    Serial.println("Hello, this is a test message!");
    delay(1000);  // Wait 1 second
}
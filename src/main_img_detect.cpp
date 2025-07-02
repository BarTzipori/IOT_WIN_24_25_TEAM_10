#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
 // Include the Arduino framework
// Wi-Fi credentials
#include "camera.h"
const char* ssid = "CS_conference";
const char* password = "openday2024";

// Server address
/// extern const char* serverUrl = "http://192.168.1.87:5015/process";
const char* testUrl = "http://httpbin.org/post";




void setup() {
  Serial.begin(115200);
delay(5000);
Serial.println("Starting setup");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");


 Serial.print("ESP32 IP Address: ");
Serial.println(WiFi.localIP());

  setupCamera();
}








void loop() {
  Serial.println("LOOP BEGIN: Sending image to server...");
  Serial.print("Wi-Fi signal strength (RSSI): ");
Serial.println(WiFi.RSSI());

  sendImageToServer();
  Serial.println("LOOP END: Image sent to server successfully?");
  delay(1000);
  Serial.println("1...");
    delay(1000);
    Serial.println("2...");
    delay(1000);
    Serial.println("3...");
    delay(1000);
    Serial.println("4...");
    delay(1000);
    Serial.println("5...");
    delay(1000);
    Serial.println("6...");
    delay(1000);
    Serial.println("7...");
    delay(1000);
    Serial.println("8...");
    delay(1000);
    Serial.println("9...");
    delay(1000);
    Serial.println("10...");
    delay(1000);
    Serial.println("11...");
    delay(1000);
    Serial.println("12...");
    delay(1000);
    Serial.println("13...");
    delay(1000);
    Serial.println("14...");
    delay(1000);
    Serial.println("15...");
}
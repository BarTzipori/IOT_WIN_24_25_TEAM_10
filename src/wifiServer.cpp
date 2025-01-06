#include "wifiServer.h"
#include "RedMP3.h"
#include <WebServer.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include "vibrationMotor.h"
#include "parameters.h"


MP3 mp3_forServer(MP3_RX, MP3_TX);

WebServer server(80);

vibrationMotor vibMotor(MOTOR_1_PIN);

void handleRoot()
{
    Serial.println("connected to ESP32");
    server.send(200, "text/plain", "ESP32 is online!");
}

void playSound1() {
  // Add code to play sound 1
  Serial.println("played sound 1");
  Serial.println("playing sound");
  mp3_forServer.setVolume(0x1e);
  mp3_forServer.playWithFileName(0x06,0x01);
  server.send(200, "text/plain", "Sound 1 played");
}

void playSound2() {
  // Add code to play sound 2
  mp3_forServer.setVolume(0x1e);
  mp3_forServer.playWithFileName(0x06,0x02);
  server.send(200, "text/plain", "Sound 2 played");
}

void playSound3() {
  // Add code to play sound 3
  mp3_forServer.setVolume(0x1e);
  mp3_forServer.playWithFileName(0x06,0x03);
  
  server.send(200, "text/plain", "Sound 3 played");
}

void playVibrationShort() {
  Serial.println("played vibration short");
  vibMotor.vibrate(vibrationPattern::shortBuzz);
  server.send(200, "text/plain", "Vibration short played");
}

void playVibrationLong() {
  Serial.println("played vibration long");
  vibMotor.vibrate(vibrationPattern::longBuzz);
  server.send(200, "text/plain", "Vibration long played");
}

void playVibrationDouble() {
  Serial.println("played vibration double");
  vibMotor.vibrate(vibrationPattern::doubleBuzz);
  server.send(200, "text/plain", "Vibration double played");
}

void playVibrationPulse() {
  Serial.println("played vibration pulse");
  
  vibMotor.vibrate(vibrationPattern::pulseBuzz);
  server.send(200, "text/plain", "Vibration pulse played");
}

void handleNotFound() {
  String message = "404 Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.print(message);
  server.send(404, "text/plain", message);
}

void onSave(){
  save_flag = true;

}

void setupWifiServer()
{
  server.on("/",handleRoot);
  server.on("/play_sound=Sound%201", playSound1);
  server.on("/play_sound=Sound%202", playSound2);
  server.on("/play_sound=Sound%203", playSound3);
  server.on("/play_vibration=Short", playVibrationShort);
  server.on("/play_vibration=Long", playVibrationLong);
  server.on("/play_vibration=Double", playVibrationDouble);
  server.on("/play_vibration=Pulse", playVibrationPulse);
  server.on("/save_settings", onSave);

  // Fallback for undefined routes
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
 
 void wifiServerLoop()
 {
    server.handleClient();
 }
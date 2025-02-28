#include "wifiServer.h"
#include "RedMP3.h"
#include <WebServer.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include "vibrationMotor.h"
#include "parameters.h"
#include "systemSettings.h"
#include "commHelperFunctions.h"
#include "sdCardHelperFunctions.h"

extern systemSettings system_settings;
FirebaseData firebaseData;

//MP3 mp3_forServer(MP3_RX, MP3_TX);
MP3 mp3(MP3_RX, MP3_TX);
WebServer server(APP_SERVER_PORT);

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
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,COLLISION_WARNING_HEBREW);
  server.send(200, "text/plain", "Sound 1 played");
}

void playSound2() {
  // Add code to play sound 2
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,ALARM_CLOCK_4_BEEPS);
  server.send(200, "text/plain", "Sound 2 played");
}

void playSound3() {
  // Add code to play sound 3
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,ALERT_1);
  
  server.send(200, "text/plain", "Sound 3 played");
}
void playSound4() {
  // Add code to play sound 3
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,ALERT_2);
  
  server.send(200, "text/plain", "Sound 4 played");
}
void playSound5() {
  // Add code to play sound 3
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP);
  
  server.send(200, "text/plain", "Sound 5 played");
}
void playSound6() {
  // Add code to play sound 3
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,WARNING_BEEPS);
  
  server.send(200, "text/plain", "Sound 6 played");
}
void playSound7() {
  // Add code to play sound 3
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,NOTIFICATION_SOUND);
  
  server.send(200, "text/plain", "Sound 7 played");
}
void playSound8() {
  // Add code to play sound 3
  mp3.setVolume(system_settings.getVolumeFormat());
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP_BEEP);
  
  server.send(200, "text/plain", "Sound 8 played");
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

void playVolume1() {
  // Add code to play sound 3
  mp3.setVolume(VOLUME_1);
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP);
  
  server.send(200, "text/plain", "Volume 1 played");
}

void playVolume2() {
  // Add code to play sound 3
  mp3.setVolume(VOLUME_2);
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP);
  
  server.send(200, "text/plain", "Volume 2 played");
}

void playVolume3() {
  // Add code to play sound 3
  mp3.setVolume(VOLUME_3);
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP);
  
  server.send(200, "text/plain", "Volume 3 played");
}

void playVolume4() {
  // Add code to play sound 3
  mp3.setVolume(VOLUME_4);
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP);
  
  server.send(200, "text/plain", "Volume 4 played");
}

void playVolume5() {
  // Add code to play sound 3
  mp3.setVolume(VOLUME_5);
  mp3.playWithFileName(ALERTS_DIR,BEEP_BEEP);
  
  server.send(200, "text/plain", "Volume 5 played");
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
  //save_flag = true;
  
  systemSettings s;
  if (getFirebaseSettings(&firebaseData, s))
  {
    
  system_settings.updateSettings(s);
  updateSDSettings(system_settings);
  system_settings.changeVolume(system_settings.getVolume(), &mp3);
  Serial.println("Settings updated");
  system_settings.print();
  server.send(200, "text/plain", "Settings saved");
  }
  else
  {
    Serial.println("Failed to get settings from Firebase");
    server.send(200, "text/plain", "Failed to save settings");
  }
}

void setupWifiServer()
{
  server.on("/",handleRoot);
  server.on("/play_sound=Collision_warning_hebrew", playSound1);
  server.on("/play_sound=Alarm_clock_4_beeps", playSound2);
  server.on("/play_sound=Alert1", playSound3);
  server.on("/play_sound=Alert2", playSound4);
  server.on("/play_sound=Beep_2", playSound5);
  server.on("/play_sound=Warning_beeps", playSound6);
  server.on("/play_sound=Notification_sound", playSound7);
  server.on("/play_sound=Beep_3", playSound8);
  server.on("/play_vibration=Short", playVibrationShort);
  server.on("/play_vibration=Long", playVibrationLong);
  server.on("/play_vibration=Double", playVibrationDouble);
  server.on("/play_vibration=Pulse", playVibrationPulse);
  server.on("/play_sound=1", playVolume1);
  server.on("/play_sound=2", playVolume2);
  server.on("/play_sound=3", playVolume3);
  server.on("/play_sound=4", playVolume4);
  server.on("/play_sound=5", playVolume5);

  server.on("/settings", onSave);

  // Fallback for undefined routes
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("HTTP Server Available at: http://" + WiFi.localIP().toString());
  
}
 
 void wifiServerLoop()
 {
    server.handleClient();
 }
#ifndef _wifi_Server_H__
#define _wifi_Server_H__

#include <WiFi.h>
#include <WebServer.h>
#include <SoftwareSerial.h>
#include "RedMP3.h"
#include "parameters.h"

void setupWifiServer();
void handleRoot();
void playSound1();
void playSound2();
void playSound3();
void playSound4();
void playSound5();
void playSound6();
void playSound7();
void playSound8();
void playVolume1();
void playVolume2();
void playVolume3();
void playVolume4();
void playVolume5();
void playVibrationShort();
void playVibrationLong();
void playVibrationDouble();
void playVibrationPulse();
void handleNotFound();
void wifiServerLoop();

#endif
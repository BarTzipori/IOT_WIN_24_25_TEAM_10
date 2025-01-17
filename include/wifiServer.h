#ifndef _wifi_Server_H__
#define _wifi_Server_H__

#include <WiFi.h>
#include <WebServer.h>
#include <SoftwareSerial.h>
#include "RedMP3.h"



void setupWifiServer();
void handleRoot();
void playSound1();
void playSound2();
void playSound3();
void handleNotFound();
void wifiServerLoop();

#endif
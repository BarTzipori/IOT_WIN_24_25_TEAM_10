#ifndef _msg_Server_H__
#define _msg_Server_H__

#include <Arduino.h>
#include <WebSerial.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void onReceive(uint8_t *data, size_t len);
void setupMsgServer();
void msgServerLoop();

#endif
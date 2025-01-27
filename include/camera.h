#ifndef _SS_CAMERA_H__
#define _SS_CAMERA_H__

#include "esp_camera.h"
#include "parameters.h"
#include <Arduino.h>
#include "SECRETS.h"
#include "parameters.h"
#include <Firebase_ESP_Client.h>
#include "base64.h" 
#include "SD_MMC.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include "systemSettings.h"
#include "safestep_logs.h"

bool setupCamera();
bool CaptureAndUploadImage(FirebaseData &fbdo,FirebaseAuth &auth, FirebaseConfig &config);
bool savePictureToSD(camera_fb_t *fb,bool wifi_flag);
camera_fb_t *capturePicture();
//void uploadImageToFirebase(const char *filePath, FirebaseData &fbdo);
//void uploadImageToRealtimeDB(const char *filePath, FirebaseData &fbdo);
//void uploadImageToRealtimeDBInChunks(const char *filePath, FirebaseData &fbdo);
//void sendImageToServer();
String FormatTime(unsigned long currentMillis,bool wifi_flag,bool sd_flag);
bool CaptureObstacle(FirebaseData &fbdo,FirebaseAuth &auth, FirebaseConfig &config,bool wifi_flag);
#endif
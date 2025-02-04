#ifndef COMMHELPERFUNCTIONS_H
#define COMMHELPERFUNCTIONS_H

#include <Firebase_ESP_Client.h>
#include "SECRETS.h"
#include "parameters.h"
#include "systemSettings.h"
#include <time.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <Arduino.h>
#include <sd_read_write.h>


void setupFirebase(FirebaseConfig &config , FirebaseAuth &auth);
bool getFirebaseSettings(FirebaseData *firebaseData, systemSettings &s);
void storeFirebaseSetting(FirebaseData *firebaseData ,systemSettings& s);
bool updateFirebaseLocalIP(FirebaseData *firebaseData, String localIP);
bool WifiSetup();
bool WifiManagerSetup();
void setupTime();
bool uploadLogs(fs::FS &fs, FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config);
bool uploadLogToFirebaseStorage(fs::FS &fs, FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config,  String &localFilePath,  String &remoteFilePath);
bool uploadImages(fs::FS &fs, FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config);
bool uploadImagesToFirebaseStorage(fs::FS &fs, FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config, String &localFilePath, String &remoteFilePath);
#endif
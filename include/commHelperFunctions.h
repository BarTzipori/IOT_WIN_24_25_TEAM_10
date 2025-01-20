#ifndef COMMHELPERFUNCTIONS_H
#define COMMHELPERFUNCTIONS_H

#include <Firebase_ESP_Client.h>
#include "SECRETS.h"
#include "systemSettings.h"
#include <time.h>
#include <WiFiManager.h>


void setupFirebase(FirebaseConfig &config , FirebaseAuth &auth);
bool getFirebaseSettings(FirebaseData *firebaseData, systemSettings &s);
void storeFirebaseSetting(FirebaseData *firebaseData ,systemSettings& s);
bool updateFirebaseLocalIP(FirebaseData *firebaseData, String localIP);
bool WifiSetup();
bool WifiManagerSetup();
void setupTime();

#endif
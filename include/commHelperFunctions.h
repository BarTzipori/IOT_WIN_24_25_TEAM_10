#ifndef COMMHELPERFUNCTIONS_H
#define COMMHELPERFUNCTIONS_H

#include <Firebase_ESP_Client.h>
#include "SECRETS.h"
#include "systemSettings.h"
#include <time.h>


void setupFirebase(FirebaseConfig &config , FirebaseAuth &auth);
systemSettings getFirebaseSettings(FirebaseData *firebaseData);
void storeFirebaseSetting(FirebaseData *firebaseData ,systemSettings& s);
bool WifiSetup(unsigned long &startTime, unsigned long &currTime);
void setupTime();

#endif
#ifndef COMMHELPERFUNCTIONS_H
#define COMMHELPERFUNCTIONS_H

#include <FirebaseESP32.h>
#include "SECRETS.h"
#include "systemSettings.h"



void setupFirebase(FirebaseConfig &config , FirebaseAuth &auth);
systemSettings getFirebaseSettings(FirebaseData &firebaseData);
void storeFirebaseSetting(FirebaseData &firebaseData ,systemSettings& s);
bool WifiSetup(unsigned long &startTime, unsigned long &currTime);

#endif
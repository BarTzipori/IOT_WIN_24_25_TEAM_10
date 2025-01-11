#include "commHelperFunctions.h"

void setupFirebase(FirebaseConfig &config , FirebaseAuth &auth) {
    config.api_key = API_KEY;
    config.database_url = FIREBASE_HOST;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    Firebase.begin(&config, &auth);
    // Ensure the network is reconnected
    Firebase.reconnectNetwork(true);
}

systemSettings getFirebaseSettings(FirebaseData &firebaseData) {
  Serial.println("Getting settings from firebase...");
  String mode, viberation, sound;
  double timming;
  int user_height, system_height, volume;
  if (Firebase.getString(firebaseData, "/System_Settings/settings/vibrationType"))
  {
    viberation = firebaseData.stringData();
  } else {
    Serial.print("Failed to get viberation: ");
    Serial.println(firebaseData.errorReason());
}

if (Firebase.getString(firebaseData, "/System_Settings/settings/soundType")) {
    sound = firebaseData.stringData();
    //Serial.println("Sound: " + sound);
  } else {
      Serial.print("Failed to get sound: ");
      Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/settings/soundOrVibration")) {
    mode = firebaseData.stringData();
    //Serial.println("Mode: " + mode);
  } else {
      Serial.print("Failed to get mode: "); 
      Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/settings/notificationTiming")) {
    timming = firebaseData.doubleData();
    //Serial.println("Timing: " + timming);
  } else {
      Serial.print("Failed to get notification timing: ");
      Serial.println(firebaseData.errorReason());
  }
  
  if (Firebase.getString(firebaseData, "/System_Settings/settings/userHeight")) {
      user_height = firebaseData.intData();
    } else {
      Serial.print("Failed to get user height: ");
      Serial.println(firebaseData.errorReason());
  }
  if (Firebase.getString(firebaseData, "/System_Settings/settings/systemHeight")) {
      system_height = firebaseData.intData();
    } else {
      Serial.print("Failed to get user height: ");
      Serial.println(firebaseData.errorReason());
  }
  if (Firebase.getInt(firebaseData, "/System_Settings/Volume")) {
    volume = firebaseData.intData();
    } else {
      Serial.print("Failed to get notification volume: ");
      Serial.println(firebaseData.errorReason());
      volume = 0;
    }
  return systemSettings(mode, sound, viberation, timming, user_height, system_height, volume);
}

void storeFirebaseSetting(FirebaseData &firebaseData ,systemSettings& s)
{
   if (Firebase.setString(firebaseData, "/System_Settings/settings/soundType", s.getSound())) {
      Serial.println("sound stored successfully");
    } else {
      Serial.print("Error storing sound: ");
      Serial.println(firebaseData.errorReason());
    }

  if (Firebase.setString(firebaseData, "/System_Settings/settings/vibrationType", s.getViberation())) {
    Serial.println("vibration stored successfully");
  } else {
    Serial.print("Error storing vibration: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/settings/soundOrVibration", s.getMode())) {
    Serial.println("mode stored successfully");
  } else {
    Serial.print("Error storing mode: ");
    Serial.println(firebaseData.errorReason());
  }

   if (Firebase.setString(firebaseData, "/System_Settings/settings/notificationTiming",s.getTiming())) {
    Serial.println("timing stored successfully");
  } else {
    Serial.print("Error storing timing: ");
    Serial.println(firebaseData.errorReason());
  }

   if (Firebase.setDouble(firebaseData, "/System_Settings/settings/userHeight", s.getUserHeight())) {
    Serial.println("User height stored successfully");
  } else {
    Serial.print("Error storing user height: ");
    Serial.println(firebaseData.errorReason());
  }
  if (Firebase.setDouble(firebaseData, "/System_Settings/settings/systemHeight", s.getSystemHeight())) {
    Serial.println("System height stored successfully");
  } else {
    Serial.print("Error storing system height: ");
    Serial.println(firebaseData.errorReason());
  }
  if (Firebase.setInt(firebaseData, "/System_Settings/settings/Volume", s.getVolume())) {
    Serial.println("volume stored successfully");
  } else {
    Serial.print("Error storing volume: ");
    Serial.println(firebaseData.errorReason());
  }
}

bool WifiSetup(unsigned long &startTime, unsigned long &currTime)
{
    startTime = millis();
    currTime = millis();
    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED && currTime - startTime < WIFI_TIMEOUT)
    {
        delay(1000);
        Serial.print(".");
        currTime = millis();
        // Serial.println(currTime - startTime);
  }
  if(WiFi.status()==WL_CONNECTED){
      Serial.println("\nConnected to Wi-Fi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      return true;
  } else {
        Serial.println("\nNot Connected to Wi-Fi!");
        return false;
  }
}
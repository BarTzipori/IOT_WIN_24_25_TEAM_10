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
  
  String mode, viberation, timming, sound;
  double user_height, system_height;
  int volume;
  if (Firebase.getString(firebaseData, "/System_Settings/Vibration"))
  {
    viberation = firebaseData.stringData();
  } else {
    Serial.print("Failed to get viberation: ");
    Serial.println(firebaseData.errorReason());
}

if (Firebase.getString(firebaseData, "/System_Settings/Sound")) {
    sound = firebaseData.stringData();
    //Serial.println("Sound: " + sound);
  } else {
      Serial.print("Failed to get sound: ");
      Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/Mode")) {
    mode = firebaseData.stringData();
    //Serial.println("Mode: " + mode);
  } else {
      Serial.print("Failed to get mode: "); 
      Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/Timing")) {
    timming = firebaseData.stringData();
    //Serial.println("Timing: " + timming);
  } else {
      Serial.print("Failed to get timing: ");
      Serial.println(firebaseData.errorReason());
  }
  
    if (Firebase.getDouble(firebaseData, "/System_Settings/User Height")) {
    user_height = firebaseData.doubleData();
    //Serial.print("Height: ");
    // Serial.println(height);
    } else {
      Serial.print("Failed to get user height: ");
      Serial.println(firebaseData.errorReason());
  }
  if (Firebase.getInt(firebaseData, "/System_Settings/Volume")) {
    volume = firebaseData.intData();
    //Serial.print("Volume: ");
    // Serial.println(volume);
    } else {
      Serial.print("Failed to get volume: ");
      Serial.println(firebaseData.errorReason());
      volume = 0;
    }

  return systemSettings(mode, sound, viberation, timming, user_height, system_height, volume);
}

void storeFirebaseSetting(FirebaseData &firebaseData ,systemSettings& s)
{
   if (Firebase.setString(firebaseData, "/System_Settings/Sound", s.getSound())) {
      Serial.println("sound stored successfully");
    } else {
      Serial.print("Error storing sound: ");
      Serial.println(firebaseData.errorReason());
    }

  if (Firebase.setString(firebaseData, "/System_Settings/Vibration", s.getViberation())) {
    Serial.println("vibration stored successfully");
  } else {
    Serial.print("Error storing vibration: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/Mode", s.getMode())) {
    Serial.println("mode stored successfully");
  } else {
    Serial.print("Error storing mode: ");
    Serial.println(firebaseData.errorReason());
  }

   if (Firebase.setString(firebaseData, "/System_Settings/Timing",s.getTiming())) {
    Serial.println("timing stored successfully");
  } else {
    Serial.print("Error storing timing: ");
    Serial.println(firebaseData.errorReason());
  }

   if (Firebase.setDouble(firebaseData, "/System_Settings/User Height", s.getUserHeight())) {
    Serial.println("User height stored successfully");
  } else {
    Serial.print("Error storing user height: ");
    Serial.println(firebaseData.errorReason());
  }
  if (Firebase.setDouble(firebaseData, "/System_Settings/System Height", s.getSystemHeight())) {
    Serial.println("System height stored successfully");
  } else {
    Serial.print("Error storing system height: ");
    Serial.println(firebaseData.errorReason());
  }
  if (Firebase.setInt(firebaseData, "/System_Settings/Volume", s.getVolume())) {
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
        Serial.println("\nNot Connected!");
        return false;
  }
}
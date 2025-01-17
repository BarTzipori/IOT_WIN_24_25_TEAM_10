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


bool getFirebaseSettings(FirebaseData *firebaseData, systemSettings &s) {
    Serial.println("Getting settings from Firebase...");

    String mode= "Both", alert_method= "timeToImpact" ,sound_1 = "Sound1", sound_2 = "Sound1", sound_3 = "Sound1", vibration_1 = "vibration1", vibration_2 = "vibration1", vibration_3 = "vibration1", voice_alerts_language = "English";
    int userheight = 170, systemheight = 85, volume = 5, distance_1 = 1000, distance_2 = 500, distance_3 = 250;
    bool enable_alert_1 = true, enable_alert_2 = true, enable_alert_3 = true, enable_voice_alerts = true,enable_camera = true;
    double timing_1 = 1.5, timing_2 = 0.8, timing_3 = 0.3;

    // Helper macro for fetching data from Firebase
#define GET_STRING(path, target)                     \
    if (Firebase.RTDB.getString(firebaseData, path)) \
    {                                                \
        target = firebaseData->to<const char *>();   \
    }                                                \
    else                                             \
    {                                                \
        Serial.print("Failed to get ");              \
        Serial.print(path);                          \
        Serial.print(": ");                          \
        Serial.println(firebaseData->errorReason()); \
        return false;                                \
}

#define GET_INT(path, target) \
        if (Firebase.RTDB.getInt(firebaseData, path)) { \
            target = firebaseData->to<int>(); \
        } else { \
            Serial.print("Failed to get "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
            return false; \
        }

    #define GET_DOUBLE(path, target) \
        if (Firebase.RTDB.getDouble(firebaseData, path)) { \
            target = firebaseData->to<double>(); \
        } else { \
            Serial.print("Failed to get "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
            return false; \
        }

    #define GET_BOOL(path, target) \
        if (Firebase.RTDB.getBool(firebaseData, path)) { \
            target = firebaseData->to<bool>(); \
        } else { \
            Serial.print("Failed to get "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
            return false; \
        }

    // Fetching settings
    GET_STRING("/System_Settings/settings/mode", mode);
    GET_STRING("/System_Settings/settings/method", alert_method);
    GET_STRING("/System_Settings/settings/alertSound1", sound_1);
    GET_STRING("/System_Settings/settings/alertSound2", sound_2);
    GET_STRING("/System_Settings/settings/alertSound3", sound_3);
    GET_STRING("/System_Settings/settings/alertVibration1", vibration_1);
    GET_STRING("/System_Settings/settings/alertVibration2", vibration_2);
    GET_STRING("/System_Settings/settings/alertVibration3", vibration_3);
    GET_STRING("/System_Settings/settings/voiceAlertsLanguage", voice_alerts_language);

    GET_DOUBLE("/System_Settings/settings/alertTiming1", timing_1);
    GET_DOUBLE("/System_Settings/settings/alertTiming2", timing_2);
    GET_DOUBLE("/System_Settings/settings/alertTiming3", timing_3);

    GET_DOUBLE("/System_Settings/settings/alertDistance1", distance_1);
    GET_DOUBLE("/System_Settings/settings/alertDistance2", distance_2);
    GET_DOUBLE("/System_Settings/settings/alertDistance3", distance_3);

    GET_INT("/System_Settings/settings/userHeight", userheight);
    GET_INT("/System_Settings/settings/systemHeight", systemheight);
    GET_INT("/System_Settings/settings/volume", volume);

    GET_BOOL("/System_Settings/settings/enableAlert1", enable_alert_1);
    GET_BOOL("/System_Settings/settings/enableAlert2", enable_alert_2);
    GET_BOOL("/System_Settings/settings/enableAlert3", enable_alert_3);
    GET_BOOL("/System_Settings/settings/enableVoiceAlerts", enable_voice_alerts);
    GET_BOOL("/System_Settings/settings/enableCamera", enable_camera);

    // Construct the systemSettings object with the fetched values
    systemSettings settings(mode, alert_method, enable_alert_1, enable_alert_2, enable_alert_3,
                            sound_1, sound_2, sound_3,
                            vibration_1, vibration_2, vibration_3,
                            timing_1, timing_2, timing_3,
                            distance_1, distance_2, distance_3,
                            userheight, systemheight,
                            enable_voice_alerts, voice_alerts_language, volume, enable_camera);

    Serial.println("Settings retrieved successfully.");
    s.updateSettings(settings);
    return true;
}


void storeFirebaseSetting(FirebaseData *firebaseData, systemSettings &s) {
    Serial.println("Storing settings to Firebase...");

    // Helper macro for setting data to Firebase
    #define SET_STRING(path, value) \
        if (Firebase.RTDB.setString(firebaseData, path, value)) { \
            Serial.print(path); \
            Serial.println(" stored successfully."); \
        } else { \
            Serial.print("Error storing "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
        }

    #define SET_DOUBLE(path, value) \
        if (Firebase.RTDB.setDouble(firebaseData, path, value)) { \
            Serial.print(path); \
            Serial.println(" stored successfully."); \
        } else { \
            Serial.print("Error storing "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
        }

    #define SET_INT(path, value) \
        if (Firebase.RTDB.setInt(firebaseData, path, value)) { \
            Serial.print(path); \
            Serial.println(" stored successfully."); \
        } else { \
            Serial.print("Error storing "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
        }

    #define SET_BOOL(path, value) \
        if (Firebase.RTDB.setBool(firebaseData, path, value)) { \
            Serial.print(path); \
            Serial.println(" stored successfully."); \
        } else { \
            Serial.print("Error storing "); \
            Serial.print(path); \
            Serial.print(": "); \
            Serial.println(firebaseData->errorReason()); \
        }

    // Store settings to Firebase
    SET_STRING("/System_Settings/settings/mode", s.getMode());
    SET_STRING("/System_Settings/settings/method", s.getAlertMethod());
    SET_STRING("/System_Settings/settings/alertSound1", s.getAlertSound1());
    SET_STRING("/System_Settings/settings/alertSound2", s.getAlertSound2());
    SET_STRING("/System_Settings/settings/alertSound3", s.getAlertSound3());
    SET_STRING("/System_Settings/settings/alertVibration1", s.getAlertVibration1());
    SET_STRING("/System_Settings/settings/alertVibration2", s.getAlertVibration2());
    SET_STRING("/System_Settings/settings/alertVibration3", s.getAlertVibration3());
    SET_STRING("/System_Settings/settings/voiceAlertsLanguage", s.getVoiceAlertsLanguage());

    SET_DOUBLE("/System_Settings/settings/alertTiming1", s.getAlertTiming1());
    SET_DOUBLE("/System_Settings/settings/alertTiming2", s.getAlertTiming2());
    SET_DOUBLE("/System_Settings/settings/alertTiming3", s.getAlertTiming3());

    SET_DOUBLE("/System_Settings/settings/alertDistance1", s.getAlertDistance1());
    SET_DOUBLE("/System_Settings/settings/alertDistance2", s.getAlertDistance2());
    SET_DOUBLE("/System_Settings/settings/alertDistance3", s.getAlertDistance3());

    SET_INT("/System_Settings/settings/userHeight", s.getUserHeight());
    SET_INT("/System_Settings/settings/systemHeight", s.getSystemHeight());
    SET_INT("/System_Settings/settings/volume", s.getVolume());

    SET_BOOL("/System_Settings/settings/enableAlert1", s.getEnableAlert1());
    SET_BOOL("/System_Settings/settings/enableAlert2", s.getEnableAlert2());
    SET_BOOL("/System_Settings/settings/enableAlert3", s.getEnableAlert3());
    SET_BOOL("/System_Settings/settings/enableVoiceAlerts", s.getEnableVoiceAlerts());
    SET_BOOL("/System_Settings/settings/enableCamera", s.getEnableCamera());

    Serial.println("Settings stored successfully.");
}


bool updateFirebaseLocalIP(FirebaseData *firebaseData, String localIP) {
    Serial.println("Updating local IP to Firebase...");

    if (Firebase.RTDB.setString(firebaseData, "/System_Settings/localIP", localIP)) {
        Serial.println("Local IP stored successfully.");
        return true;
    } else {
        Serial.print("Error storing local IP: ");
        Serial.println(firebaseData->errorReason());
        return false;
    }
}


bool WifiSetup()
{
    // New wifi setup
    WiFiManager wm;
    wm.setConnectTimeout(15);
    wm.setConfigPortalTimeout(180);
    bool res = wm.autoConnect("SafeStepAP", "safestep2025");
    if (!res)
    {
        Serial.println("Failed to connect");
        return false;
    }
    else
    {
        Serial.println("Connected to Wi-Fi");
        Serial.print("local ip: ");
        Serial.println(WiFi.localIP());
        return true;
    }

    //old wifi setup
    /*
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
  */
}


void setupTime()
{
    // Set the timezone to Israel (UTC+2) and adjust for DST (+1 hour during DST)
  const long gmtOffset_sec = 7200; // UTC+2 in seconds
  const int daylightOffset_sec = 3600; // +1 hour for DST
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  // Get and print the local time
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("Time synchronized:");
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S"); // Print the formatted time
  } else {
    Serial.println("Failed to obtain time");
  }
}
#include "commHelperFunctions.h"

//extern bool time_flag;
extern Flags flags;

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
    s.setMode(mode);
    GET_STRING("/System_Settings/settings/method", alert_method);
    s.setAlertMethod(alert_method);
    GET_STRING("/System_Settings/settings/alertSound1", sound_1);
    s.setAlertSound1(sound_1);
    GET_STRING("/System_Settings/settings/alertSound2", sound_2);
    s.setAlertSound2(sound_2);
    GET_STRING("/System_Settings/settings/alertSound3", sound_3);
    s.setAlertSound3(sound_3);
    GET_STRING("/System_Settings/settings/alertVibration1", vibration_1);
    s.setAlertVibration1(vibration_1);
    GET_STRING("/System_Settings/settings/alertVibration2", vibration_2);
    s.setAlertVibration2(vibration_2);
    GET_STRING("/System_Settings/settings/alertVibration3", vibration_3);
    s.setAlertVibration3(vibration_3);
    GET_STRING("/System_Settings/settings/voiceAlertsLanguage", voice_alerts_language);
    s.setVoiceAlertsLanguage(voice_alerts_language);

    GET_DOUBLE("/System_Settings/settings/alertTiming1", timing_1);
    s.setAlertTiming1(timing_1);
    GET_DOUBLE("/System_Settings/settings/alertTiming2", timing_2);
    s.setAlertTiming2(timing_2);
    GET_DOUBLE("/System_Settings/settings/alertTiming3", timing_3);
    s.setAlertTiming3(timing_3);

    GET_DOUBLE("/System_Settings/settings/alertDistance1", distance_1);
    s.setAlertDistance1(distance_1);
    GET_DOUBLE("/System_Settings/settings/alertDistance2", distance_2);
    s.setAlertDistance2(distance_2);
    GET_DOUBLE("/System_Settings/settings/alertDistance3", distance_3);
    s.setAlertDistance3(distance_3);

    GET_INT("/System_Settings/settings/userHeight", userheight);
    s.setUserHeight(userheight);
    GET_INT("/System_Settings/settings/systemHeight", systemheight);
    s.setSystemHeight(systemheight);
    GET_INT("/System_Settings/settings/volume", volume);
    s.setVolume(volume);

    GET_BOOL("/System_Settings/settings/enableAlert1", enable_alert_1);
    s.setEnableAlert1(enable_alert_1);
    GET_BOOL("/System_Settings/settings/enableAlert2", enable_alert_2);
    s.setEnableAlert2(enable_alert_2);
    GET_BOOL("/System_Settings/settings/enableAlert3", enable_alert_3);
    s.setEnableAlert3(enable_alert_3);
    GET_BOOL("/System_Settings/settings/enableVoiceAlerts", enable_voice_alerts);
    s.setEnableVoiceAlerts(enable_voice_alerts);
    GET_BOOL("/System_Settings/settings/enableCamera", enable_camera);
    s.setEnableCamera(enable_camera);

    
    Serial.println("Settings retrieved successfully.");
    
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


bool WifiManagerSetup()
{
    // New wifi setup
    WiFiManager wm;
    Preferences preferences;
    preferences.begin("wifi-data", false);
    // wm.setConnectTimeout(10);
    wm.setConfigPortalTimeout(30);
    // bool res = wm.autoConnect("SafeStepAP", "safestep2025");
    bool res = wm.startConfigPortal("SafeStepAP", "safestep2025");
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
        String ssid = WiFi.SSID(); // Get the SSID of the connected network
        if (!ssid.isEmpty())
        {
            preferences.putString("savedSSID", ssid);
            Serial.println("Saved SSID to NVS: " + ssid);
        }
        else
        {
            Serial.println("no Wi-Fi configured.");
        }

        return true;
    }
}

bool WifiSetup()
{
    Preferences preferences;
    preferences.begin("wifi-data", false);
    unsigned long startTime = millis();
    unsigned long currTime = millis();
    // Connect to Wi-Fi
    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.mode(WIFI_STA);
    WiFi.begin();

    Serial.println("Connecting to Wi-Fi");
    String ssid = preferences.getString("savedSSID", "No SSID saved");
    if (ssid.length() > 0)
    {
        Serial.print("Attempting to connect to Wi-Fi network: ");
        Serial.print(ssid);
    }
    else
    {
        Serial.println("No saved Wi-Fi credentials found.");
    }
    // Serial.print("Trying to Connect to "+ String(WiFi.SSID()));
    while (WiFi.status() != WL_CONNECTED && currTime - startTime < WIFI_TIMEOUT)
    {
        delay(1000);
        Serial.print(".");
        currTime = millis();
        // Serial.println(currTime - startTime);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConnected to Wi-Fi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    else
    {
        Serial.println("\nNot Connected to Wi-Fi!");
        WiFi.disconnect(true);
        return false;
    }
}


void setupTime()
{
    //time_flag = false;
    // Set the timezone to Israel (UTC+2) and adjust for DST (+1 hour during DST)
    const long gmtOffset_sec = 7200;     // UTC+2 in seconds
    const int daylightOffset_sec = 3600; // +1 hour for DST
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

    // Get and print the local time
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        flags.time_flag = true;
        Serial.println("Time synchronized:");
        Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S"); // Print the formatted time
  } else {
    Serial.println("Failed to obtain time");
  }
}
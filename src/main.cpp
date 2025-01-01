#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino.h>
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#include "sd_read_write.h"
#include "SD_MMC.h"
#include "settings.h"


#define SD_MMC_CMD 38 //Please do not modify it.
#define SD_MMC_CLK 39 //Please do not modify it. 
#define SD_MMC_D0  40 //Please do not modify it.

#define WIFI_TIMEOUT 1500

// Replace with your network credentials
#define WIFI_SSID "Matans iPhone"
#define WIFI_PASSWORD "12345678B"

// Firebase credentials
#define FIREBASE_HOST "https://safestep-2bc31-default-rtdb.europe-west1.firebasedatabase.app" // Replace with your database URL
#define API_KEY "AIzaSyBplNk42Hp3O6M17iJObv_FM2MuanadKjM"

#define USER_EMAIL "neta.matano@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData firebaseData; // Firebase object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


unsigned long startTime,currTime;

bool setupSDCard()
{
        SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
      Serial.println("Card Mount Failed");
      return false;
    }
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return false;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
    return true;
}

bool WifiSetup()
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
      return true;
  }
      else {
        Serial.println("\nNot Connected!");
        return false;
      }
}

void init_sd_card()
{
  if (isExist(SD_MMC, "/Settings", "setting.txt"))
          Serial.println("setting file exist!");
          else  {
              createDir(SD_MMC, "/Settings");
              writeFile(SD_MMC, "/Settings/setting.txt", "Mode: Both\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Sound: Sound_1\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Viberation: Viberation_1\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Timing: 0.5s\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Height: 1.65\n");
              endFile(SD_MMC, "/Settings/setting.txt");
              Serial.println("created setting file");
                    }
}

void setupFirebase(){
  config.api_key = API_KEY;
  config.database_url = FIREBASE_HOST;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  config.token_status_callback = tokenStatusCallback;
}

settings getFirebaseSettings()
{
  
  String mode, viberation, timming, sound;
  double height;
  if (Firebase.getString(firebaseData, "/System_Settings/Vibration"))
  {
    viberation = firebaseData.stringData()
  } else {
    Serial.println("Failed to get viberation: " + firebaseData.errorReason());
}

if (Firebase.getString(firebaseData, "/System_Settings/Sound")) {
    sound = firebaseData.stringData();
    //Serial.println("Sound: " + sound);
  } else {
      Serial.println("Failed to get sound: " + firebaseData.errorReason());
  }


  if (Firebase.getString(firebaseData, "/System_Settings/Mode")) {
    mode = firebaseData.stringData();
    //Serial.println("Mode: " + mode);
  } else {
      Serial.println("Failed to get mode: " + firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/Timing")) {
    timming = firebaseData.stringData();
    //Serial.println("Timing: " + timming);
  } else {
      Serial.println("Failed to get timing: " + firebaseData.errorReason());
  }
  
    if (Firebase.getDouble(firebaseData, "/System_Settings/Height")) {
    height = firebaseData.doubleData();
    //Serial.print("Height: ");
    // Serial.println(height);
    } else {
      Serial.println("Failed to get height: " + firebaseData.errorReason());
  }
  return settings(mode, sound, viberation, timming, height);
}

void storeFirebaseSetting(settings s)
{
   if (Firebase.setString(firebaseData, "/System_Settings/Sound", s.Sound))
 {
   Serial.println("sound stored successfully");
  } else {
    Serial.println("Error storing sound: " + firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/Vibration", s.Viberation)) {
    Serial.println("vibration stored successfully");
  } else {
    Serial.println("Error storing vibration: " + firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/Mode", s.Mode)) {
    Serial.println("mode stored successfully");
  } else {
    Serial.println("Error storing mode: " + firebaseData.errorReason());
  }

   if (Firebase.setString(firebaseData, "/System_Settings/Timing",s.timing)) {
    Serial.println("timing stored successfully");
  } else {
    Serial.println("Error storing timing: " + firebaseData.errorReason());
  }

   if (Firebase.setInt(firebaseData, "/System_Settings/Height", s.height)) {
    Serial.println("height stored successfully");
  } else {
    Serial.println("Error storing height: " + firebaseData.errorReason());
  }
}

bool wifi_flag;
settings setting;


void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("Begin");

  
  // Connect to Wi-Fi
 wifi_flag = WifiSetup();

  // Initialize Firebase
 setupSDCard();
 init_sd_card();
 setting = readSettings(SD_MMC, "/Settings/setting.txt");
 if (wifi_flag){
   setupFirebase();
   settings fb_setting = getFirebaseSettings();
   setting.updateSettings(fb_setting);
 }
}


void loop() {
  // Nothing here
  

  //delay(10000);
}

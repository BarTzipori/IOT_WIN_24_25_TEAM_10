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

bool wifi_flag;

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Begin");

  
  // Connect to Wi-Fi
 wifi_flag = WifiSetup();

  // Initialize Firebase
      setupSDCard();
      if (isExist(SD_MMC, "/Settings", "setting.txt"))
          Serial.println("setting file exist!");
          else  {
              createDir(SD_MMC, "/Settings");
              writeFile(SD_MMC, "/Settings/setting.txt", "Mode: Both\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Sound: Sound_1\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Viberation: Viberation_1\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Timing: 0.5s\n");
              appendFile(SD_MMC, "/Settings/setting.txt", "Height: 1.65\n");
              Serial.println("created setting file");
          }
          readFile(SD_MMC, "/Settings/setting.txt");

          config.api_key = API_KEY;
          config.database_url = FIREBASE_HOST;

          auth.user.email = USER_EMAIL;
          auth.user.password = USER_PASSWORD;

          Firebase.begin(&config, &auth);
          Firebase.reconnectWiFi(true);

          config.token_status_callback = tokenStatusCallback;

          // Store multiple string variables in Firebase
          String var1 = "Sound_1";
          String var2 = "Viberation_1";
          String var3 = "Both";
          String var4 = "0.5s";
          double var5 = 1.68;

          // Push data to Firebase
          if (Firebase.setString(firebaseData, "/System_Settings/Sound", var1))
          {
              Serial.println("var1 stored successfully");
  } else {
    Serial.println("Error storing var1: " + firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/Vibration", var2)) {
    Serial.println("var2 stored successfully");
  } else {
    Serial.println("Error storing var2: " + firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/Mode", var3)) {
    Serial.println("var3 stored successfully");
  } else {
    Serial.println("Error storing var3: " + firebaseData.errorReason());
  }

   if (Firebase.setString(firebaseData, "/System_Settings/Timing", var4)) {
    Serial.println("var4 stored successfully");
  } else {
    Serial.println("Error storing var4: " + firebaseData.errorReason());
  }

   if (Firebase.setInt(firebaseData, "/System_Settings/Height", var5)) {
    Serial.println("var5 stored successfully");
  } else {
    Serial.println("Error storing var5: " + firebaseData.errorReason());
  }
}
String mode, viberation, timming, sound;
double height;

void loop() {
  // Nothing here
  if (Firebase.getString(firebaseData, "/System_Settings/Vibration")) {
    viberation = firebaseData.stringData();
    Serial.println("Viberation: " + viberation);
  } else {
    Serial.println("Failed to get var1: " + firebaseData.errorReason());
}

if (Firebase.getString(firebaseData, "/System_Settings/Sound")) {
    sound = firebaseData.stringData();
    Serial.println("Sound: " + sound);
  } else {
      Serial.println("Failed to get var2: " + firebaseData.errorReason());
  }


  if (Firebase.getString(firebaseData, "/System_Settings/Mode")) {
    mode = firebaseData.stringData();
    Serial.println("Mode: " + mode);
  } else {
      Serial.println("Failed to get var3: " + firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/Timing")) {
    timming = firebaseData.stringData();
    Serial.println("Timing: " + timming);
  } else {
      Serial.println("Failed to get var4: " + firebaseData.errorReason());
  }
  
    if (Firebase.getDouble(firebaseData, "/System_Settings/Height")) {
    height = firebaseData.doubleData();
    Serial.print("Height: ");
     Serial.println(height);
    } else {
      Serial.println("Failed to get var4: " + firebaseData.errorReason());
  }

  delay(10000);
}

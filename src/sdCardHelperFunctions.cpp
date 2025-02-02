#include "sdCardHelperFunctions.h"

extern String log_filename;

bool setupSDCard() {
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    delay(100);
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
    createDir(SD_MMC, "/images");
    return true;
}

bool init_sd_card()
{
    bool flag = false;
    if (isExist(SD_MMC, "/Settings", "setting.txt"))
    {
        Serial.println("setting file exist!");
        flag= true;
    }
    else
    {
        createDir(SD_MMC, "/Settings");
        writeFile(SD_MMC, "/Settings/setting.txt", "Mode: Both");
        appendFile(SD_MMC, "/Settings/setting.txt", "Method: TimeToImpact");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_sound_1: Beep_2");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_sound_2: Alarm_clock_4_beeps");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_sound_3: Collision_warning_hebrew");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_Vibration_1: Short");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_Vibration_2: Double");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_Vibration_3: Pulse");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_timing_1: 1.5");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_timing_2: 0.8");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_timing_3: 0.3");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_distance_1: 100");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_distance_2: 50");
        appendFile(SD_MMC, "/Settings/setting.txt", "alert_distance_3: 25");
        appendFile(SD_MMC, "/Settings/setting.txt", "user_height: 170");
        appendFile(SD_MMC, "/Settings/setting.txt", "system_height: 85");
        appendFile(SD_MMC, "/Settings/setting.txt", "minimum_obstacle_height: 85");
        appendFile(SD_MMC, "/Settings/setting.txt", "head_clearance: 5");
        appendFile(SD_MMC, "/Settings/setting.txt", "enable_alert_1: true");
        appendFile(SD_MMC, "/Settings/setting.txt", "enable_alert_2: true");
        appendFile(SD_MMC, "/Settings/setting.txt", "enable_alert_3: true");
        appendFile(SD_MMC, "/Settings/setting.txt", "enable_voice_alerts: true");
        appendFile(SD_MMC, "/Settings/setting.txt", "voice_alerts_language: English");
        appendFile(SD_MMC, "/Settings/setting.txt", "volume: 5");
        appendFile(SD_MMC, "/Settings/setting.txt", "enable_camera: true");
        endFile(SD_MMC, "/Settings/setting.txt");
        Serial.println("created setting file");
        flag = false;
    }

    return flag;
}

bool init_logs(bool wifi_flag)
{
    bool flag;
    if (!isExistFolder(SD_MMC, "/logs"))
    {
        Serial.println("Creating logs folder");
        createDir(SD_MMC, "/logs");
        flag = true;
    }

    String filename = "/logs/log_" + FormatTime(millis(), wifi_flag, true) + ".txt";
    writeFile(SD_MMC, filename.c_str(), "Log file created at: " + FormatTime(millis(), wifi_flag, true));
    endFile(SD_MMC, filename.c_str());
    Serial.print("created log file ");
    Serial.println(filename);
    log_filename = filename;

    return flag;
}
//updates system settings on the SD card
void updateSDSettings(systemSettings &s)
{
    deleteFile(SD_MMC, "/Settings/setting.txt");
    String mode("Mode: ");
    mode = mode + s.getMode();
    writeFile(SD_MMC, "/Settings/setting.txt", mode);
    appendFile(SD_MMC, "/Settings/setting.txt", "Method: " + s.getAlertMethod());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_sound_1: " + s.getAlertSound1());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_sound_2: " + s.getAlertSound2());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_sound_3: " + s.getAlertSound3());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_Vibration_1: " + s.getAlertVibration1());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_Vibration_2: " + s.getAlertVibration2());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_Vibration_3: " + s.getAlertVibration3());
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_timing_1: " + String(s.getAlertTiming1()));
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_timing_2: " + String(s.getAlertTiming2()));
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_timing_3: " + String(s.getAlertTiming3()));
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_distance_1: " + String(s.getAlertDistance1()));
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_distance_2: " + String(s.getAlertDistance2()));
    appendFile(SD_MMC, "/Settings/setting.txt", "alert_distance_3: " + String(s.getAlertDistance3()));
    appendFile(SD_MMC, "/Settings/setting.txt", "user_height: " + String(s.getUserHeight()));
    appendFile(SD_MMC, "/Settings/setting.txt", "system_height: " + String(s.getSystemHeight()));
    appendFile(SD_MMC, "/Settings/setting.txt", "minimum_obstacle_height: " + String(s.getMinimumObstacleHeight()));
    appendFile(SD_MMC, "/Settings/setting.txt", "head_clearance: " + String(s.getHeadClearance()));
    appendFile(SD_MMC, "/Settings/setting.txt", "enable_alert_1: " + String(s.getEnableAlert1()));
    appendFile(SD_MMC, "/Settings/setting.txt", "enable_alert_2: " + String(s.getEnableAlert2()));
    appendFile(SD_MMC, "/Settings/setting.txt", "enable_alert_3: " + String(s.getEnableAlert3()));
    appendFile(SD_MMC, "/Settings/setting.txt", "enable_voice_alerts: " + String(s.getEnableVoiceAlerts()));
    appendFile(SD_MMC, "/Settings/setting.txt", "voice_alerts_language: " + s.getVoiceAlertsLanguage());
    appendFile(SD_MMC, "/Settings/setting.txt", "volume: " + String(s.getVolume()));
    appendFile(SD_MMC, "/Settings/setting.txt", "enable_camera: " + String(s.getEnableCamera()));

    endFile(SD_MMC, "/Settings/setting.txt");
}

#include "sdCardHelperFunctions.h"

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
    return true;
}

bool init_sd_card()
{
  if (isExist(SD_MMC, "/Settings", "setting.txt")){
    Serial.println("setting file exist!");
    return true;
  }
    else  {
        createDir(SD_MMC, "/Settings");
        writeFile(SD_MMC, "/Settings/setting.txt", "Mode: Both\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Sound: Sound_1\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Viberation: Viberation_1\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Timing: 0.5s\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Height: 1.65\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Volume: 5\n");
        endFile(SD_MMC, "/Settings/setting.txt");
        Serial.println("created setting file");
        return false;
    }
}
//updates system settings on the SD card
void updateSDSettings(systemSettings &s)
{
    deleteFile(SD_MMC, "/Settings/setting.txt");
    String mode("Mode: ");
    mode = mode + s.getMode();
    writeFile(SD_MMC, "/Settings/setting.txt", mode);
    String sound("Sound: ");
    sound = sound + s.getSound();
    appendFile(SD_MMC, "/Settings/setting.txt", sound);
    String vibration("Vibration: ");
    vibration = vibration + s.getViberation();
    appendFile(SD_MMC, "/Settings/setting.txt", vibration);
    String timing("Timing: ");
    timing = timing + s.getTiming();
    appendFile(SD_MMC, "/Settings/setting.txt", timing);
    String height("Height: ");
    height = height + String(s.getHeight());
    appendFile(SD_MMC, "/Settings/setting.txt", height);
    String volume("Volume: ");
    volume = volume + String(s.getVolume());
    appendFile(SD_MMC, "/Settings/setting.txt", volume);
    endFile(SD_MMC, "/Settings/setting.txt");
}

#include "RedMP3.h"
#include "parameters.h"
#include "voiceAlertsAsTasks.h"

void playPoweringOnsystemAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, POWERING_ON_SYSTEM);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}
void playSystemReadytoUseAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, SYSTEM_READY_TO_USE);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playSilentModeEnabledAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, SILENT_MODE_ACTIVATED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playSilentModeDisabledAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, SILENT_MODE_DEACTIVATED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}
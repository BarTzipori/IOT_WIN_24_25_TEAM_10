#include "RedMP3.h"
#include "parameters.h"
#include "voiceAlertsAsTasks.h"

void playPoweringOnSystemAsTask(void *pvParameters) {
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

void playWifiConnectedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, WIFI_CONNECTED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playWifiNotConnectedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, WIFI_NOT_CONNECTED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playWifiPairingInitiatedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, WIFI_PAIRING_INITIATED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playPleaseConnectToSafestepWifiAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, PLEASE_CONNECT_TO_SAFESTEP_WIFI);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playSystemPairedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, SYSTEM_PAIRED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playSystemNotPairedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, SYSTEM_NOT_PAIRED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playNoSDDetectedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, NO_SD_DETECTED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playNoSDAndWifiAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, NO_SD_AND_WIFI);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playNoCameraDetectedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, NO_CAMERA_DETECTED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playMPUCalibrationStartAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, MPU_CALIBRATION_START);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playGyroCalibrationStartAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, GYRO_CALIBRATION_START);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playMagCalibrationStartAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, MAG_CALIBRATION_START);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playMPUCalibrationDoneAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, MPU_CALIBRATION_DONE);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playMPUSensorDegradedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, MPU_SENSOR_DEGRADED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playDistanceSensorDegradedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, DISTANCE_SENSOR_DEGRADED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playUploadLogsAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, UPLOAD_LOGS);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playUploadingFilesAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, UPLOADING_FILES);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

void playErrorReportedAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(VOICE_ALERTS_DIR, ERROR_REPORTED);
  vTaskDelay(1000);
  vTaskDelete(NULL);
}

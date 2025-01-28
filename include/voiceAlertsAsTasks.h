
#ifndef VOICEALERTSASTASKS_H
#define VOICEALERTSASTASKS_H

#include "RedMP3.h"
#include "parameters.h"

void playPoweringOnSystemAsTask(void *pvParameters);
void playSystemReadytoUseAsTask(void *pvParameters);
void playSilentModeEnabledAsTask(void *pvParameters);
void playSilentModeDisabledAsTask(void *pvParameters);
void playWifiConnectedAsTask(void *pvParameters);
void playWifiNotConnectedAsTask(void *pvParameters);
void playWifiPairingInitiatedAsTask(void *pvParameters);
void playNoSDDetectedAsTask(void *pvParameters);
void playNoSDAndWifiAsTask(void *pvParameters);
void playNoCameraDetectedAsTask(void *pvParameters);
void playMPUCalibrationStartAsTask(void *pvParameters);
void playGyroCalibrationStartAsTask(void *pvParameters);
void playMagCalibrationStartAsTask(void *pvParameters);
void playMPUCalibrationDoneAsTask(void *pvParameters);
void playMPUSensorDegradedAsTask(void *pvParameters);
void playDistanceSensorDegradedAsTask(void *pvParameters);
void playUploadLogsAsTask(void *pvParameters);
void playUploadingFilesAsTask(void *pvParameters);
void playErrorReportedAsTask(void *pvParameters);
void playPleaseConnectToSafestepWifiAsTask(void *pvParameters);
void playSystemPairedAsTask(void *pvParameters);
void playSystemNotPairedAsTask(void *pvParameters);

#endif
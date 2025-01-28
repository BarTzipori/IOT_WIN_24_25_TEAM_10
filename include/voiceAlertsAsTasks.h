
#ifndef VOICEALERTSASTASKS_H
#define VOICEALERTSASTASKS_H

#include "RedMP3.h"
#include "parameters.h"

void playPoweringOnsystemAsTask(void *pvParameters);
void playSystemReadytoUseAsTask(void *pvParameters);
void playSilentModeEnabledAsTask(void *pvParameters);
void playSilentModeDisabledAsTask(void *pvParameters);

#endif
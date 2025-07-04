#ifndef _SAFESTEP_LOGS_H_
#define _SAFESTEP_LOGS_H_

#include <Arduino.h>
#include "sd_read_write.h"
#include "SD_MMC.h"
#include "camera.h"
#include "systemSettings.h"

void logData(String data);
void sendDistanceData(const std::vector<std::pair<int, int>>& distances, String target_ip);

#endif
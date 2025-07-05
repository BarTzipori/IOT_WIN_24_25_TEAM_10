#ifndef _OBSTACLE_IDENTIFIER_ALERTS_H
#define _OBSTACLE_IDENTIFIER_ALERTS_H

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include "RedMP3.h"

void playObstacleAlertHoleAsTask(void *pvParameters);
void playObstacleAlertPotholeAsTask(void *pvParameters);
void playObstacleAlertFenceAsTask(void *pvParameters);
void playOBstacleAlertBarrierAsTask(void *pvParameters);
void playOBstacleAlertWallAsTask(void *pvParameters);
void playObstacleAlertStepAsTask(void *pvParameters);
void playObstacleAlertStairsAsTask(void *pvParameters);
void playObstacleAlertCurbAsTask(void *pvParameters);
void playObstacleAlertBranchAsTask(void *pvParameters);
void playObstacleAlertTreeAsTask(void *pvParameters);
void playObstacleAlertBikeAsTask(void *pvParameters);
void playObstacleAlertScooterAsTask(void *pvParameters);
void playObstacleAlertCarAsTask(void *pvParameters);
void playObstacleAlertTrashAsTask(void *pvParameters);
void playObstacleAlertSignAsTask(void *pvParameters);
void playObstacleAlertConstructionAsTask(void *pvParameters);
void playObstacleAlertBlockAsTask(void *pvParameters);
void playObstacleAlertPoleAsTask(void *pvParameters);
void playObstacleAlertChairAsTask(void *pvParameters);
void playObstacleAlertTableAsTask(void *pvParameters);
void playObstacleAlertDoorAsTask(void *pvParameters);
void playObstacleAlertGateAsTask(void *pvParameters);
void playObstacleAlertBenchAsTask(void *pvParameters);
void playObstacleAlertWindowAsTask(void *pvParameters);
void playObstacleAlertFloorAsTask(void *pvParameters);
void playObstacleAlertLedgeAsTask(void *pvParameters);
void playObstacleAlertDropAsTask(void *pvParameters);
void playObstacleAlertManAsTask(void *pvParameters);
void playObstacleAlertDeskAsTask(void *pvParameters);
void playObstacleAlertRefrigeratorAsTask(void *pvParameters);
void playObstacleAlertPersonAsTask(void *pvParameters);
void playObstacleAlertBinAsTask(void *pvParameters);
void playObstacleAlertBoxAsTask(void *pvParameters);
void playObstacleAlertWomanAsTask(void *pvParameters);

void playAnalyzingEnvironmentAstask(void *pvParameters);

void playObstacleAlertsByNames(const std::vector<std::string>& obstacleNames, MP3* mp3);

#endif // _OBSTACLE_IDENTIFIER_ALERTS_H
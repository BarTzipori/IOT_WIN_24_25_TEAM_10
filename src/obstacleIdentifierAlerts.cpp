
#include "RedMP3.h"
#include "parameters.h"
#include "obstacleIdentifierAlerts.h"

void playObstacleAlertHoleAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_HOLE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertPotholeAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_POTHOLE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertFenceAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_FENCE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playOBstacleAlertBarrierAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BARRIER_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playOBstacleAlertWallAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_WALL_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertStepAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_STEP_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertStairsAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_STAIRS_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertCurbAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_CURB_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertBranchAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BRANCH_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertTreeAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_TREE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}               
void playObstacleAlertBikeAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BIKE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertScooterAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_SCOOTER_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertCarAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_CAR_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertTrashAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_TRASH_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertSignAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_SIGN_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertConstructionAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_CONSTRUCTION_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertBlockAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BLOCK_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertPoleAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_POLE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertChairAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_CHAIR_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertTableAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_TABLE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertDoorAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_DOOR_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertGateAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_GATE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertBenchAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BENCH_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertWindowAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_WINDOW_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertFloorAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_FLOOR_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertLedgeAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_LEDGE_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertDropAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_DROP_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertManAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_MAN_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertDeskAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_DESK_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertRefrigeratorAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_REFRIGERATOR_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertPersonAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_PERSON_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertBinAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BIN_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertBoxAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_BOX_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}
void playObstacleAlertWomanAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, OBSTACLE_WOMAN_ALERT);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}

void playNoIdentifiableObjectFoundAsTask(void *pvParameters) {
  MP3 *mp3 = (MP3 *)pvParameters; // Cast the incoming parameter to an array of void pointers
  mp3->playWithFileName(OBSTACLE_IDENTIFIER_ALERTS_DIR, NO_OBSTACLE_IDENTIFIED);
  vTaskDelay(pdMS_TO_TICKS(2000)); // 1000 ms = 1 second
  vTaskDelete(NULL);
}

void playObstacleAlertsByNames(const std::vector<std::string>& obstacleNames, MP3* mp3) {
  if(obstacleNames.empty()) {
      xTaskCreate(playNoIdentifiableObjectFoundAsTask, "playNoIdentifiableObjectFoundAsTask", STACK_SIZE, &mp3, 2, nullptr);
      return;
  } else {
      static const std::unordered_map<std::string, std::function<void(void*)>> obstacleAlertMap = {
        {"hole", playObstacleAlertHoleAsTask},
        {"pothole", playObstacleAlertPotholeAsTask},
        {"fence", playObstacleAlertFenceAsTask},
        {"barrier", playOBstacleAlertBarrierAsTask},
        {"wall", playOBstacleAlertWallAsTask},
        {"step", playObstacleAlertStepAsTask},
        {"stairs", playObstacleAlertStairsAsTask},
        {"curb", playObstacleAlertCurbAsTask},
        {"branch", playObstacleAlertBranchAsTask},
        {"tree", playObstacleAlertTreeAsTask},
        {"bike", playObstacleAlertBikeAsTask},
        {"scooter", playObstacleAlertScooterAsTask},
        {"car", playObstacleAlertCarAsTask},
        {"trash", playObstacleAlertTrashAsTask},
        {"sign", playObstacleAlertSignAsTask},
        {"construction", playObstacleAlertConstructionAsTask},
        {"block", playObstacleAlertBlockAsTask},
        {"pole", playObstacleAlertPoleAsTask},
        {"chair", playObstacleAlertChairAsTask},
        {"table", playObstacleAlertTableAsTask},
        {"door", playObstacleAlertDoorAsTask},
        {"gate", playObstacleAlertGateAsTask},
        {"bench", playObstacleAlertBenchAsTask},
        {"window", playObstacleAlertWindowAsTask},
        {"floor", playObstacleAlertFloorAsTask},
        {"ledge", playObstacleAlertLedgeAsTask},
        {"drop", playObstacleAlertDropAsTask},
        {"man", playObstacleAlertManAsTask},
        {"desk", playObstacleAlertDeskAsTask},
        {"refrigerator", playObstacleAlertRefrigeratorAsTask},
        {"person", playObstacleAlertPersonAsTask},
        {"bin", playObstacleAlertBinAsTask},
        {"box", playObstacleAlertBoxAsTask},
        {"woman", playObstacleAlertWomanAsTask}
      };
      
      for (const auto& name : obstacleNames) {
          auto it = obstacleAlertMap.find(name);
          if (it != obstacleAlertMap.end()) {
              it->second(static_cast<void*>(mp3));
          }
      }
    }
}
// Note: The above function assumes that the MP3 object is properly initialized and passed as a parameter.
// The vTaskDelay function is used to pause the task for a specified duration, allowing the MP3 alert to play before the task is deleted.
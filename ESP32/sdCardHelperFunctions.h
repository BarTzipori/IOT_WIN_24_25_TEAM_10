#ifndef SD_CARD_HELPER_FUNCTIONS_H
#define SD_CARD_HELPER_FUNCTIONS_H

#include "parameters.h"
#include "sd_read_write.h"
#include "SD_MMC.h"
#include "systemSettings.h"
#include "camera.h"
//#include "webmsg.h"

bool setupSDCard();
bool init_sd_card();
bool init_logs(bool wifi_flag);
void updateSDSettings(systemSettings &s);

#endif // SD_CARD_HELPER_FUNCTIONS_H
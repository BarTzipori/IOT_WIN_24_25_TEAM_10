#include "safestep_logs.h"
#include "WebSerial.h"

String log_filename;
extern Flags flags;

void logData(String data)
{
    Serial.println(data);
    String stamp = FormatTime(millis(),flags.wifi_flag,false) + " :  ";
    if(flags.sd_flag)
        (SD_MMC, log_filename.c_str(), stamp + data);
    if (flags.wifi_flag)
    {
        WebSerial.println(data);
    }
}
#include "safestep_logs.h"
#include "WebSerial.h"

String log_filename;
//bool wifi_flag;
extern Flags flags;

void logData(String data)
{
    String stamp = FormatTime(millis(),flags.wifi_flag,false) + " :  ";
    appendFile(SD_MMC, log_filename.c_str(), stamp + data);
    if(flags.wifi_flag){
        Serial.println("got here");
        WebSerial.println(stamp + data);
    }

    
}
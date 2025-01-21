#include "safestep_logs.h"
#include "WebSerial.h"

String log_filename;

void logData(String data)
{
    appendFile(SD_MMC, log_filename.c_str(), data);
    WebSerial.println(data);
    
}
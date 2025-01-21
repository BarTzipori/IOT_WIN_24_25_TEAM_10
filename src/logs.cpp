#include "logs.h"

String log_filename;

void logData(String data)
{
    appendFile(SD_MMC, log_filename.c_str(), data);
    WebSerial.println(data);
    
}
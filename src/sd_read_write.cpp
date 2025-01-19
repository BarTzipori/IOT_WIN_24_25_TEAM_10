#include "sd_read_write.h"
#include <string.h>
#include "systemSettings.h"
#include <vector>

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    String t1(message);
    String t2("\n");
    String msg = String(t1+t2);
    if (file.print(msg))
    {
        Serial.println("File written");
        file.close();
    }
    else
    {
        Serial.println("Write failed");
    }
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    String t1(message);
    String t2("\n");
    String msg = String(t1+t2);
    if (file.print(msg)){
        Serial.println("Message appended");
        file.close();
    }
    else
    {
        Serial.println("Append failed");
    }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}


bool isExist(fs::FS &fs ,const char * dirname,const char * filename)
{
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return false;
    }

        File file = root.openNextFile();
    while(file){
        //Serial.println(file.name());
       // Serial.println(filename);
      //  Serial.println(strcmp(file.name(),filename));
        if(file.isDirectory()){
            continue;
        } else {
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
            if(strcmp(file.name(),filename)==0){
                file.close();
                return true;
            }
        }
        file = root.openNextFile();
}
file.close();
return false;
}

systemSettings readSettings(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);
    std::vector<String> tokens;
    String m_mode, m_method, m_sound1,m_sound2,m_sound3,m_viberation1,m_viberation2,m_viberation3,m_language;
    double m_timing1,m_timing2,m_timing3;
    int m_usrheight,m_sysheight,m_volume, m_distance1,m_distance2,m_distance3;
    bool m_enable_alert1,m_enable_alert2,m_enable_alert3,m_enable_voice_alerts,m_enable_camera;
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return systemSettings();
    }
    systemSettings s;
    Serial.println("Read from file: ");
    while(file.available()){
        //int tmp = file.read();
        //Serial.printf("%c",tmp);

        //Serial.write(file.read());

        String line = file.readStringUntil('\n'); // Read until the newline character
        tokens = parseString(line);

        Serial.println(line);

       //for (String str : tokens)
         //  Serial.println(str);

        if (tokens[0]=="Mode:") {
               m_mode = tokens[1];
               s.setMode(m_mode);
               continue;
        }

        if (tokens[0]=="Method:") {
               m_method = tokens[1];
               s.setAlertMethod(m_method);
               continue;
        }

        if (tokens[0]=="alert_sound_1:"){
               m_sound1 = tokens[1];
               s.setAlertSound1(m_sound1);
               continue;
        }

        if (tokens[0]=="alert_sound_2:"){
               m_sound2 = tokens[1];
                s.setAlertSound2(m_sound2);
               continue;
        }

        if (tokens[0]=="alert_sound_3:"){
               m_sound3 = tokens[1];
                s.setAlertSound3(m_sound3);
               continue;
        }

        if (tokens[0]=="alert_Vibration_1:"){
               m_viberation1 = tokens[1];
                s.setAlertVibration1(m_viberation1);
               continue;
        }

        if (tokens[0]=="alert_Vibration_2:"){
               m_viberation2 = tokens[1];
                s.setAlertVibration2(m_viberation2);
               continue;
        }

        if (tokens[0]=="alert_Vibration_3:"){
               m_viberation3 = tokens[1];
                s.setAlertVibration3(m_viberation3);
               continue;
        }

        if (tokens[0]=="alert_timing_1:") {
               m_timing1 = tokens[1].toDouble();
                s.setAlertTiming1(m_timing1);
               continue;
        }

        if (tokens[0]=="alert_timing_2:") {
               m_timing2 = tokens[1].toDouble();
                s.setAlertTiming2(m_timing2);
               continue;
        }

        if (tokens[0]=="alert_timing_3:") {
               m_timing3 = tokens[1].toDouble();
                s.setAlertTiming3(m_timing3);
               continue;
        }

        if (tokens[0]=="alert_distance_1:") {
            m_distance1 = tokens[1].toInt();
            s.setAlertDistance1(m_distance1);
            continue;
        }
        if (tokens[0]=="alert_distance_2:") {
            m_distance2 = tokens[1].toInt();
            s.setAlertDistance2(m_distance2);
            continue;
        }
        if (tokens[0]=="alert_distance_3:") {
            m_distance3 = tokens[1].toInt();
            s.setAlertDistance3(m_distance3);
            continue;
        }
        if (tokens[0]=="user_height:") {
               m_usrheight = tokens[1].toInt();
                s.setUserHeight(m_usrheight);
               continue;
        }

        if (tokens[0]=="system_height:") {
               m_sysheight = tokens[1].toInt();
                s.setSystemHeight(m_sysheight);
               continue;
        }

        if (tokens[0]=="enable_alert_1:") {
               m_enable_alert1 = tokens[1].toInt();
                s.setEnableAlert1(m_enable_alert1);
               continue;
        }

        if (tokens[0]=="enable_alert_2:") {
               m_enable_alert2 = tokens[1].toInt();
                s.setEnableAlert2(m_enable_alert2);
               continue;
        }

        if (tokens[0]=="enable_alert_3:") {
               m_enable_alert3 = tokens[1].toInt();
                s.setEnableAlert3(m_enable_alert3);
               continue;
        }

        if (tokens[0]=="enable_voice_alerts:")
           {
               m_enable_voice_alerts = tokens[1].toInt();
                s.setEnableVoiceAlerts(m_enable_voice_alerts);
               continue;
           }

        if (tokens[0]=="voice_alerts_language:")
           {
               m_language = tokens[1];
                s.setVoiceAlertsLanguage(m_language);
               continue;
           }


        if (tokens[0]=="volume:")
           {
               m_volume = tokens[1].toInt();
                s.setVolume(m_volume);
               continue;
           }

        if (tokens[0]=="enable_camera:")
           {
               m_volume = tokens[1].toInt();
                s.setEnableCamera(m_volume);
               continue;
           }
    }
    file.close();
    return s;
}

void endFile(fs::FS &fs,const char *path)
{
    File file = fs.open(path);
    file.write(0x1A); // Control-Z, often used as an EOF marker

    file.close();
}

std::vector<String> parseString(String input)
{
    std::vector<String> tmp;
    while (input.length() > 0)
    {
        int spaceIndex = input.indexOf(' '); // Find the first space
        if (spaceIndex == -1) {
            // No more spaces, print the remaining part
            //Serial.println(input);
            tmp.push_back(input);
            break;
        }

        String token = input.substring(0, spaceIndex); // Extract the token
        //Serial.println(token); // Print the token
        tmp.push_back(token);
        input = input.substring(spaceIndex + 1); // Update the string, remove the token
    }
    return tmp;
}


void writeFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    String t1(message);
    String t2("\n");
    String msg = String(t1+t2);
    if (file.print(msg))
    {
        Serial.println("File written");
        file.close();
    }
    else
    {
        Serial.println("Write failed");
    }
}

void appendFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    String t1(message);
    String t2("\n");
    String msg = String(t1+t2);
    if (file.print(msg)){
        Serial.println("Message appended");
        file.close();
    }
    else
    {
        Serial.println("Append failed");
    }
}

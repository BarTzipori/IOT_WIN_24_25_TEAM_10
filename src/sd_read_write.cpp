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
    String m_mode, m_sound, m_viberation, m_timing;
    double m_user_height, m_system_height;
    int m_volume;
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return systemSettings();
    }

    Serial.println("Read from file: ");
    while(file.available()){
        //int tmp = file.read();
        //Serial.printf("%c",tmp);

        //Serial.write(file.read());

        String line = file.readStringUntil('\n'); // Read until the newline character
        tokens = parseString(line);

        Serial.println(line+"\n");

        //for (String str : tokens)
        //  Serial.println(str);

        if (tokens[0]=="Mode:") {
               m_mode = tokens[1];
        }
        if (tokens[0]=="Sound:") {
               m_sound = tokens[1];
        }
        if (tokens[0]=="Vibration:") {
            m_viberation = tokens[1];
        }
        if (tokens[0]=="Timing:") {
            m_timing = tokens[1];
        }
        if (tokens[0]=="User Height:") {
            m_user_height = tokens[1].toInt();
        }
        if (tokens[0]=="system Height:") {
            m_system_height = tokens[1].toInt();
        }
        if (tokens[0]=="Volume:") {
            m_volume = tokens[1].toInt();
        }
    }
    file.close();
    return systemSettings(m_mode,m_sound,m_viberation,m_timing,m_user_height,m_system_height,m_volume);
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

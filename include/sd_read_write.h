#ifndef _SD_READ_WRITE_H
#define _SD_READ_WRITE_H

#include "Arduino.h"
#include "FS.h"
#include "systemSettings.h"
#include <vector>

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);
bool isExist(fs::FS &fs, const char *dirname, const char *filename);
bool isExistFolder(fs::FS &fs, const char *path);
systemSettings readSettings(fs::FS &fs, const char *path);
void endFile(fs::FS &fs,const char *path);
std::vector<String> parseString(String str);
void writeFile(fs::FS &fs, const char * path, String message);
void appendFile(fs::FS &fs, const char * path, String message);
#endif

#ifndef _SETTINGS_H
#define _SETTINGS_H
#include <Arduino.h>

class settings
{
    public:
    String Mode;
    String Sound;
    String Viberation;
    String timing;
    double height;
    settings(String m, String s, String v, String t, int h);

    void updateSettings(settings s);
};

#endif
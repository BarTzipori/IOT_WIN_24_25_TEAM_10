#ifndef _SYSTEM_SETTINGS_H
#define _SYSTEM_SETTINGS_H

#include <Arduino.h>

class systemSettings
{
    private:
    String Mode;
    String Sound;
    String Vibration;
    String timing;
    int height;
    int volume;

public:
    systemSettings(): Mode("Both"), Sound("default"), Vibration("default"), timing("default"), height(0),volume(5) {};
    systemSettings(String m, String s, String v, String t, int h,int vol);
    
    String getMode() const {return Mode;}
    String getSound() const {return Sound;}
    String getViberation() const {return Vibration;}
    String getTiming() const {return timing;}
    int getHeight() const {return height;}
    int getVolume() const {return volume;}

    bool updateSettings(systemSettings s);
    void print();
};

#endif
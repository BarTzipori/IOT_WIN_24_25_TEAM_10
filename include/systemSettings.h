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
    double height;

    public:
    systemSettings(): Mode("default"), Sound("default"), Vibration("default"), timing("default"), height(0.0) {};
    systemSettings(String m, String s, String v, String t, int h);
    
    String getMode() const {return Mode;}
    String getSound() const {return Sound;}
    String getVibration() const {return Vibration;}
    String getTiming() const {return timing;}
    double getHeight() const {return height;}

    void updateSettings(systemSettings s);
    void print();
};

#endif
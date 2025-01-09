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
    double user_height;
    double system_height;
    int volume;

public:
    systemSettings(): Mode("Both"), Sound("default"), Vibration("default"), timing("default"), user_height(0.0), system_height(0.0), volume(5) {};
    systemSettings(String mode, String sound_type, String vibration_pattern, String timing, double userH, double systemH, int vol);
    
    String getMode() const {return Mode;}
    String getSound() const {return Sound;}
    String getViberation() const {return Vibration;}
    String getTiming() const {return timing;}
    double getUserHeight() const {return user_height;}
    double getSystemHeight() const {return system_height;}
    int getVolume() const {return volume;}

    bool updateSettings(systemSettings s);
    void print();
};

#endif
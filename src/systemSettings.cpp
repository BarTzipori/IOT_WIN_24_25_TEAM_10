#include "systemSettings.h"
#include <Arduino.h>
systemSettings::systemSettings(String m,String s, String v,String t,int h)
{
    Mode = m;
    Sound = s;
    Vibration = v;
    timing = t;
    height = h;
}

void systemSettings::updateSettings(systemSettings s)
{
    if(s.Mode != "default" && s.Mode != "") {
        Mode = s.Mode;
    }
    if(s.Sound != "default" && s.Sound != "") {
        Sound = s.Sound;
    }
    if(s.Vibration != "default" && s.Vibration != "") {
        Vibration = s.Vibration;
    }
    if(s.timing != "default" && s.timing != "") {
        timing = s.timing;
    }
    if(s.height != 0.0) {
        height = s.height;
    }
}

void systemSettings::print()
{
    Serial.println("Print Settings: ");
    Serial.print("Mode: ");
    Serial.println(Mode);
    Serial.print("Sound: ");
    Serial.println(Sound);
    Serial.print("Vibration: ");
    Serial.println(Vibration);
    Serial.print("Timing: ");
    Serial.println(timing);
    Serial.print("Height: ");
    Serial.println(height);
}
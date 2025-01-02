#include "systemSettings.h"
#include <Arduino.h>
systemSettings::systemSettings(String m,String s, String v,String t,int h)
{
    Mode = m;
    Sound = s;
    Viberation = v;
    timing = t;
    height = h;
}

void systemSettings::updateSettings(systemSettings s)
{
    Mode = s.Mode;
    Sound = s.Sound;
    Viberation = s.Viberation;
    timing =s.timing;
    height = s.height;
}

void systemSettings::print()
{
    Serial.println("Print Settings: ");
    Serial.print("Mode: ");
    Serial.println(Mode);
    Serial.print("Sound: ");
    Serial.println(Sound);
    Serial.print("Vibration: ");
    Serial.println(Viberation);
    Serial.print("Timing: ");
    Serial.println(timing);
    Serial.print("Height: ");
    Serial.println(height);
}
#include "systemSettings.h"
#include <Arduino.h>
systemSettings::systemSettings(String m, String s, String v, String t, double h, int vol)
{
    Mode = m;
    Sound = s;
    Vibration = v;
    timing = t;
    height = h;
    volume = vol;
}

bool systemSettings::updateSettings(systemSettings s)
{
    bool changed = false;
    if (Mode != s.Mode) {
      Mode = s.Mode;
      changed = true;
    }
    if(Sound!=s.Sound){
    Sound = s.Sound;
    changed = true;
    }
    if (Vibration != s.Vibration){
    Vibration = s.Vibration;
    changed = true;
    }
    if (timing != s.timing){
    timing =s.timing;
    changed = true;
    }
    if (height !=s.height)
    {
    height = s.height;
    changed = true;
    }
    if (volume != s.volume)
    {
    volume = s.volume;
    changed = true;
    }   
    return changed;
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
    Serial.print("Volume: ");
    Serial.println(volume);
}
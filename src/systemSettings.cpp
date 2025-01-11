#include "systemSettings.h"
#include <Arduino.h>
systemSettings::systemSettings(String mode, String sound_type, String vibration_pattern, double timing, int userH, int systemH, int vol)
{
    Mode = mode;
    Sound = sound_type;
    Vibration = vibration_pattern;
    timing = timing;
    user_height = userH;
    system_height = systemH;
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
    if (user_height !=s.user_height)
    {
    user_height = s.user_height;
    changed = true;
    }
    if (system_height !=s.system_height)
    {
    system_height = s.system_height;
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
    Serial.print("User Height: ");
    Serial.println(user_height);
    Serial.print("System Height: ");
    Serial.println(system_height);
    Serial.print("Volume: ");
    Serial.println(volume);
}
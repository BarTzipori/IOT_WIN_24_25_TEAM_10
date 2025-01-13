#include "systemSettings.h"
#include <Arduino.h>

systemSettings::systemSettings(String mode, String s1, String s2, String s3, String v1, String v2, String v3, double t1, double t2, double t3, int u, int s, bool e1, bool e2, bool e3, bool e, String l, int vol)
{
    Mode = mode;
    alert_sound_1 = s1;
    alert_sound_2 = s2;
    alert_sound_3 = s3;
    alert_Vibration_1 = v1;
    alert_Vibration_2 = v2;
    alert_Vibration_3 = v3;
    alert_timing_1 = t1;
    alert_timing_2 = t2;
    alert_timing_3 = t3;
    user_height = u;
    system_height = s;
    enable_alert_1 = e1;
    enable_alert_2 = e2;
    enable_alert_3 = e3;
    enable_voice_alerts = e;
    voice_alerts_language = l;
    volume = vol;
}

bool systemSettings::updateSettings(systemSettings s)
{
    bool changed = false;
    if (Mode != s.getMode())
    {
        Mode = s.getMode();
        changed = true;
    }
    if (alert_sound_1 != s.getAlertSound1())
    {
        alert_sound_1 = s.getAlertSound1();
        changed = true;
    }
    if (alert_sound_2 != s.getAlertSound2())
    {
        alert_sound_2 = s.getAlertSound2();
        changed = true;
    }
    if (alert_sound_3 != s.getAlertSound3())
    {
        alert_sound_3 = s.getAlertSound3();
        changed = true;
    }

    if (alert_Vibration_1 != s.getAlertVibration1())
    {
        alert_Vibration_1 = s.getAlertVibration1();
        changed = true;
    }
    if (alert_Vibration_2 != s.getAlertVibration2())
    {
        alert_Vibration_2 = s.getAlertVibration2();
        changed = true;
    }
    if (alert_Vibration_3 != s.getAlertVibration3())
    {
        alert_Vibration_3 = s.getAlertVibration3();
        changed = true;
    }

    if (alert_timing_1 != s.getAlertTiming1())
    {
        alert_timing_1 = s.getAlertTiming1();
        changed = true;
    }
    if (alert_timing_2 != s.getAlertTiming2())
    {
        alert_timing_2 = s.getAlertTiming2();
        changed = true;
    }
    if (alert_timing_3 != s.getAlertTiming3())
    {
        alert_timing_3 = s.getAlertTiming3();
        changed = true;
    }

    if (user_height != s.getUserHeight())
    {
        user_height = s.getUserHeight();
        changed = true;
    }
    if (system_height != s.getSystemHeight())
    {
        system_height = s.getSystemHeight();
        changed = true;
    }

    if (enable_alert_1 != s.getEnableAlert1())
    {
        enable_alert_1 = s.getEnableAlert1();
        changed = true;
    }
    if (enable_alert_2 != s.getEnableAlert2())
    {
        enable_alert_2 = s.getEnableAlert2();
        changed = true;
    }
    if (enable_alert_3 != s.getEnableAlert3())
    {
        enable_alert_3 = s.getEnableAlert3();
        changed = true;
    }

    if (enable_voice_alerts != s.getEnableVoiceAlerts())
    {
        enable_voice_alerts = s.getEnableVoiceAlerts();
        changed = true;
    }

    if (voice_alerts_language != s.getVoiceAlertsLanguage())
    {
        voice_alerts_language = s.getVoiceAlertsLanguage();
        changed = true;
    }

    if (volume != s.getVolume())
    {
        volume = s.getVolume();
        changed = true;
    }



    return changed;
}

void systemSettings::print()
{
    Serial.println("Print Settings: ");
    Serial.println("Mode: " + Mode);
    Serial.println("Alert Sound 1: " + alert_sound_1); 
    Serial.println("Alert Sound 2: " + alert_sound_2);
    Serial.println("Alert Sound 3: " + alert_sound_3);
    Serial.println("Alert Vibration 1: " + alert_Vibration_1);
    Serial.println("Alert Vibration 2: " + alert_Vibration_2);
    Serial.println("Alert Vibration 3: " + alert_Vibration_3);
    Serial.println("Alert Timing 1: " + String(alert_timing_1));
    Serial.println("Alert Timing 2: " + String(alert_timing_2));
    Serial.println("Alert Timing 3: " + String(alert_timing_3));
    Serial.println("User Height: " + String(user_height));
    Serial.println("System Height: " + String(system_height));
    Serial.println("Enable Alert 1: " + String(enable_alert_1));
    Serial.println("Enable Alert 2: " + String(enable_alert_2));
    Serial.println("Enable Alert 3: " + String(enable_alert_3));
    Serial.println("Enable Voice Alerts: " + String(enable_voice_alerts));
    Serial.println("Voice Alerts Language: " + voice_alerts_language);
    Serial.println("Volume: " + String(volume));
}
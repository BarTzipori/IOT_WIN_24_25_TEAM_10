#ifndef _SYSTEM_SETTINGS_H
#define _SYSTEM_SETTINGS_H

#include <Arduino.h>
#include "vibrationMotor.h"

class systemSettings
{
    private:
    String Mode;
    String alert_sound_1,alert_sound_2,alert_sound_3;
    String alert_Vibration_1,alert_Vibration_2,alert_Vibration_3;
    double alert_timing_1,alert_timing_2,alert_timing_3;
    int user_height,system_height;
    bool enable_alert_1,enable_alert_2,enable_alert_3,enable_voice_alerts,enable_camera;
    String voice_alerts_language;
    int volume;

public:
    systemSettings(): Mode("Both"),
        alert_sound_1("Sound1"),
        alert_sound_2("Sound1"),
        alert_sound_3("Sound1"),
        alert_Vibration_1("Vibration1"),
        alert_Vibration_2("Vibration1"),
        alert_Vibration_3("Vibration1"),
        alert_timing_1(1.5),
        alert_timing_2(0.8),
        alert_timing_3(0.3),
        user_height(170),
        system_height(170/2),
        enable_alert_1(true),
        enable_alert_2(true),
        enable_alert_3(true),
        enable_voice_alerts(true),
        voice_alerts_language("English"),
        volume(5),
        enable_camera(true) {};

    systemSettings(String mode,String s1,String s2,String s3,String v1,String v2,String v3,double t1,double t2,double t3,int u,int s,bool e1,bool e2,bool e3,bool e,String l,int vol,bool cam);
    
    
    String getMode() const {return Mode;}
    String getAlertSound1() const {return alert_sound_1;}
    String getAlertSound2() const {return alert_sound_2;}
    String getAlertSound3() const {return alert_sound_3;}
    String getAlertVibration1() const {return alert_Vibration_1;}
    String getAlertVibration2() const {return alert_Vibration_2;}
    String getAlertVibration3() const {return alert_Vibration_3;}
    double getAlertTiming1() const {return alert_timing_1;}
    double getAlertTiming2() const {return alert_timing_2;}
    double getAlertTiming3() const {return alert_timing_3;}
    int getUserHeight() const {return user_height;}
    int getSystemHeight() const {return system_height;}
    bool getEnableAlert1() const {return enable_alert_1;}
    bool getEnableAlert2() const {return enable_alert_2;}
    bool getEnableAlert3() const {return enable_alert_3;}
    bool getEnableVoiceAlerts() const {return enable_voice_alerts;}
    String getVoiceAlertsLanguage() const {return voice_alerts_language;}
    int getVolume() const {return volume;}
    bool getEnableCamera() const {return enable_camera;}

    void setMode(String m) {Mode = m;}
    void setAlertSound1(String s) {alert_sound_1 = s;}
    void setAlertSound2(String s) {alert_sound_2 = s;}
    void setAlertSound3(String s) {alert_sound_3 = s;}
    void setAlertVibration1(String v) {alert_Vibration_1 = v;}
    void setAlertVibration2(String v) {alert_Vibration_2 = v;}
    void setAlertVibration3(String v) {alert_Vibration_3 = v;}
    void setAlertTiming1(double t) {alert_timing_1 = t;}
    void setAlertTiming2(double t) {alert_timing_2 = t;}
    void setAlertTiming3(double t) {alert_timing_3 = t;}
    void setUserHeight(int u) {user_height = u;}
    void setSystemHeight(int s) {system_height = s;}
    void setEnableAlert1(bool e) {enable_alert_1 = e;}
    void setEnableAlert2(bool e) {enable_alert_2 = e;}
    void setEnableAlert3(bool e) {enable_alert_3 = e;}
    void setEnableVoiceAlerts(bool e) {enable_voice_alerts = e;}
    void setVoiceAlertsLanguage(String l) {voice_alerts_language = l;}
    void setVolume(int v) {volume = v;}
    void setEnableCamera(bool c) {enable_camera = c;}



    bool updateSettings(systemSettings s);
    void print();
    uint getAlertSound1AsInt();
    uint getAlertSound2AsInt();
    uint getAlertSound3AsInt();
    vibrationPattern getVibration1AsPattern();
    vibrationPattern getVibration2AsPattern();
    vibrationPattern getVibration3AsPattern();
};

#endif
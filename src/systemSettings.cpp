#include "systemSettings.h"

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

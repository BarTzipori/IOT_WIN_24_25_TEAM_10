#include "settings.h"

settings::settings(String m,String s, String v,String t,int h)
{
    Mode = m;
    Sound = s;
    Viberation = v;
    timing = t;
    height = h;
}

void settings::updateSettings(settings s)
{
    Mode = s.Mode;
    Sound = s.Sound;
    Viberation = s.Viberation;
    timing =s.timing;
    height = s.height;
}

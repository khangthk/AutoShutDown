#ifndef REGISTRY_H
#define REGISTRY_H

#include "power.h"

class Registry
{
public:
    static void AddRunAtStartup();
    static void RemoveRunAtStartup();
    static void AddSetting(Action action, int time);
    static void RemoveSetting();
    static void ReadSetting(Action &action, int &time);
};

#endif // REGISTRY_H

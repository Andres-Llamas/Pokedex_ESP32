#ifndef Pokemon_h
#define Pokemon_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Pokemon
{
private:
public:
    const char *name;
    const char *number;
    const char *entry;
    const char *height;
    const char *weight;
    const char *category;
    const char *typeMain;
    const char *typeSecond;
    Pokemon(StaticJsonDocument<512> document);
};

#endif
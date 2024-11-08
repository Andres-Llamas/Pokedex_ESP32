#ifndef Pokemon_h
#define Pokemon_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Pokemon
{
private:
public:
    String name;
    String number;
    String entry;
    String entry2; // 2nd line
    String entry3; // 3rd line
    String height;
    String weight;
    String category;    
    Pokemon(StaticJsonDocument<512> document);
};

#endif
#ifndef PokedexManager_h
#define PokedexManager_h

#include <Arduino.h>
#include "Pokemon.h"
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

class PokedexManager
{
private:
    static String GetPokedexEntryIndex();
    static StaticJsonDocument<512> jsonDoc; // Here will be stored the JSON pokemon data
public:
    static Pokemon* pokemon;        
    static String currentPokedexIndex;    
    static void InitializePokedex();
    static void OpenPokemonData();
};

#endif
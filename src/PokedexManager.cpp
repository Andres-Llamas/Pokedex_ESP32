#include "PokedexManager.h"

StaticJsonDocument<512> PokedexManager::jsonDoc;
Pokemon* PokedexManager::pokemon;
int PokedexManager::currentPokedexIndex;

void PokedexManager::InitializePokedex()
{
    currentPokedexIndex = 1; // default index in case something happens. This index indicates the pokedex index, so it starts at 1, not 0
    currentPokedexIndex = GetPokedexEntryIndex();
}

void PokedexManager::OpenPokemonData()
{
    if (pokemon != NULL)
    {
        delete pokemon;
        Serial.println("Deleting already existing pokemon");
    }
    File file = SD.open(String("/") + String(currentPokedexIndex) + "/PokemonData.json");
    if (!file)
    {
        Serial.println("Failed to open config.json");
        return;
    }
    Serial.println("Opening: " + String("/") + String(currentPokedexIndex) + "/PokemonData.json" );
    DeserializationError error = deserializeJson(jsonDoc, file);
    if (error)
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        file.close();
        return;
    }
    file.close();
    pokemon = new Pokemon(jsonDoc);
    Serial.println("Opened " + String(pokemon->name));
}

int PokedexManager::GetPokedexEntryIndex()
{
    File file = SD.open("/pokedexCurrentEntryIndex.txt");
    int currentIndex = file.readString().toInt();
    file.close();
    Serial.println("Seting current index to: " + currentIndex);
    return currentIndex;
}

//The pokemon types are the following
/*
    planta = 0
    veneno = 1
 */

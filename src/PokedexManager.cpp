#include "PokedexManager.h"

StaticJsonDocument<512> PokedexManager::jsonDoc;
Pokemon* PokedexManager::pokemon;
String PokedexManager::currentPokedexIndex;

void PokedexManager::InitializePokedex()
{
    currentPokedexIndex = "1"; // default index in case something happens
    currentPokedexIndex = GetPokedexEntryIndex();
}

void PokedexManager::OpenPokemonData()
{
    if (pokemon != NULL)
    {
        delete pokemon;
        Serial.println("Deleting already existing pokemon");
    }
    File file = SD.open(String("/") + currentPokedexIndex + "/PokemonData.json");
    if (!file)
    {
        Serial.println("Failed to open config.json");
        return;
    }
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
}

String PokedexManager::GetPokedexEntryIndex()
{
    File file = SD.open("/pokedexCurrentEntryIndex.txt");
    String currentIndex = file.readString();
    file.close();
    return currentIndex;
}
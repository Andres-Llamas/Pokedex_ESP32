#include "Pokemon.h"

Pokemon::Pokemon(StaticJsonDocument<512> document)
{
    this->name = document["name"].as<String>();
    this->number = document["number"].as<String>();
    this->entry = document["entry"].as<String>();
    this->entry2 = document["entry2"].as<String>();
    this->entry3 = document["entry3"].as<String>();
    this->height = document["height"].as<String>();
    this->weight = document["weight"].as<String>();
    this->category = document["category"].as<String>();
}
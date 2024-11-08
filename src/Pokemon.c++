#include "Pokemon.h"

Pokemon::Pokemon(StaticJsonDocument<512> document)
{

    this->name = document["name"];
    this->number = document["number"];
    this->entry = document["entry"];
}
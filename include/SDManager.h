#ifndef SDManager_h
#define SDManager_h

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Definitions.h"
#include <TFT_eSPI.h>

class SDManager
{
private:
public:
    static bool InitializeSD(int tries);    
};
#endif
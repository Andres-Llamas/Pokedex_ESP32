#ifndef InputManager_h
#define InputManager_h

#include <Arduino.h>
#include "Definitions.h"
class InputManager
{
private:
    /* data */
public:
    static bool canReadInput;
    static void ReadInput();    
    static bool OnButtonUp();
    static bool OnButtonDown();
    static bool OnButtonLeft();
    static bool OnButtonRight();
};


#endif
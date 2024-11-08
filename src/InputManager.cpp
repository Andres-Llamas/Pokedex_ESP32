#include "InputManager.h"

bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;
bool InputManager::canReadInput = false;

void InputManager::ReadInput()
{
    if (canReadInput)
    {
        if (digitalRead(BUTTON_UP) == HIGH)
        {
            upPressed = true;
            downPressed = false;
            leftPressed = false;
            rightPressed = false;
        }
        else if (digitalRead(BUTTON_DOWN) == HIGH)
        {
            bool upPressed = false;
            bool downPressed = true;
            bool leftPressed = false;
            bool rightPressed = false;
        }
        else if (digitalRead(BUTTON_LEFT) == HIGH)
        {
            bool upPressed = false;
            bool downPressed = false;
            bool leftPressed = true;
            bool rightPressed = false;
        }
        else if (digitalRead(BUTTON_RIGHT) == HIGH)
        {
            bool upPressed = false;
            bool downPressed = false;
            bool leftPressed = false;
            bool rightPressed = true;
        }
        else
        {
            bool upPressed = false;
            bool downPressed = false;
            bool leftPressed = false;
            bool rightPressed = false;
        }

        delay(200);
    }
}

bool InputManager::OnButtonUp()
{
    return upPressed;
}

bool InputManager::OnButtonDown()
{
    return downPressed;
}

bool InputManager::OnButtonLeft()
{
    return leftPressed;
}

bool InputManager::OnButtonRight()
{
    return rightPressed;
}
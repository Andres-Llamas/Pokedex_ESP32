#include "SDManager.h"

bool SDManager::InitializeSD(int tries)
{
	int triesCounter = tries;
    

	if (!SD.begin(SD_CS)) // if failed to initialize SD
	{
		Serial.println("SD card initialization failed!");		

		if (triesCounter > 0)
		{
            Serial.println("Trying again");			
			return SDManager::InitializeSD(--triesCounter);
		}

		Serial.println("Failed to initialize SD card after" + String(tries) + " tries");
		// tft.fillScreen(TFT_RED);
		// tft.drawString("SD Card Fail after" + String(tries) + " tries", 50, 50, 4);
		return false;
	}

	Serial.println("SD card initialized.");
	return true;
}
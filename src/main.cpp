#include <Arduino.h>
#include <SD.h>
#include <Audio.h>	  // ESP32-AudioI2S library
#include <TFT_eSPI.h> // TFT library
#include <SPI.h>
#include <JPEGDecoder.h>
#include <Audio.h> // ESP32-AudioI2S library
#include "PokedexManager.h"
#include "Definitions.h"
#include "SDManager.h"
#include "InputManager.h"

// Variables
TFT_eSPI tft = TFT_eSPI();
Audio audio;
int currentScreenBrightness = 4000;

// Function Declarations
void InitializeTFTDisplay();
void SDSuccessMessage();
void showBMP(const char *filename, int16_t x, int16_t y);
uint16_t read16(File &f);
uint32_t read32(File &f);
void drawSdJpeg(const char *filename, int x_center, int y_center);
void drawSdJpegCentered(const char *filename, int x_center, int y_center);
void jpegRender(int xpos, int ypos);
void jpegInfo();
void showTime(uint32_t msTime);
void DrawPokedexEntryScreen();
void RefreshPokedexScreen();
void SetAudioToPlay(const char *filename);
void drawTextInBox(String text, int x, int y, int box_width, int box_height);

void setup()
{
	Serial.begin(115200);

	// Set pins
	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(BUTTON_LEFT, INPUT);
	pinMode(BUTTON_RIGHT, INPUT);
	pinMode(AUDIO_ENABLE_PIN, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	digitalWrite(AUDIO_ENABLE_PIN, HIGH);
	audio.setPinout(AUDIO_BCLK, AUDIO_LRC, AUDIO_DIN);

	// Initialization
	analogWrite(BLUE_LED, 500);
	InitializeTFTDisplay();
	bool sdInitializationCode = SDManager::InitializeSD(4);

	// First Configuration
	PokedexManager::InitializePokedex();
	PokedexManager::OpenPokemonData();

	RefreshPokedexScreen();

	audio.setVolume(5);
}

void loop()
{
	audio.loop(); // Keep the audio playing
	InputManager::ReadInput();

	if (digitalRead(BUTTON_UP))
	{
		Serial.print("Up Pressed ");
		PokedexManager::currentPokedexIndex--; // Moving to previous pokemon
		RefreshPokedexScreen();
	}
	if (digitalRead(BUTTON_DOWN))
	{
		Serial.print("down Pressed ");
		PokedexManager::currentPokedexIndex++; // Moving to next pokemon
		RefreshPokedexScreen();
	}
	if (digitalRead(BUTTON_LEFT))
	{
		Serial.print("Left Pressed ");
		analogWrite(TFT_BL, 10);
	}
	if (digitalRead(BUTTON_RIGHT))
	{
		Serial.print("Right Pressed ");
		analogWrite(TFT_BL, 4000);
	}
}

void InitializeTFTDisplay()
{
	tft.init();
	tft.setRotation(1);							  // Adjust rotation as needed
	pinMode(TFT_BL, OUTPUT);					  // Set backlight pin as output
	analogWrite(TFT_BL, currentScreenBrightness); // Turn on the backlight
}

//& Draw Pokedex Menus ----------------------------------------------------------------------------------------------------------------------

void RefreshPokedexScreen()
{
	DrawPokedexEntryScreen();
	PokedexManager::OpenPokemonData();

	String audioFileDirection = String("/") + String(PokedexManager::currentPokedexIndex) + "/sound.wav";
	String imageFileDirection = String("/") + String(PokedexManager::currentPokedexIndex) + "/image.jpg";
	String mainTipeFileDirection = String("/") + String(PokedexManager::currentPokedexIndex) + "/type1.jpg";
	String secondTipeFileDirection = String("/") + String(PokedexManager::currentPokedexIndex) + "/type2.jpg";
	String footPrintFileDirection = String("/") + String(PokedexManager::currentPokedexIndex) + "/footprint.jpg";								

	Serial.println(PokedexManager::pokemon->number);
	Serial.println(PokedexManager::pokemon->name);
	Serial.println(PokedexManager::pokemon->category);
	delay(100);
	// Drawing on screen
	tft.setTextColor(TFT_WHITE, TFT_RED);
	tft.drawString(PokedexManager::pokemon->number, 210, 43, 4);
	tft.drawString(PokedexManager::pokemon->name, 260, 43, 4);
	tft.setTextColor(TFT_BLACK, TFT_WHITE);
	tft.drawString(PokedexManager::pokemon->category, 220, 77, 4);
	tft.drawString(PokedexManager::pokemon->entry, 63, 233, 2);
	tft.drawString(PokedexManager::pokemon->entry2, 63, 256, 2);
	tft.drawString(PokedexManager::pokemon->entry3, 63, 279, 2);
	tft.drawString("Height", 300, 150, 4);
	tft.drawString(PokedexManager::pokemon->height, 395, 150, 4);
	tft.drawString("Weight", 300, 186, 4);
	tft.drawString(PokedexManager::pokemon->weight, 395, 186, 4);

	drawSdJpegCentered(imageFileDirection.c_str(), 90, 120);
	drawSdJpegCentered(mainTipeFileDirection.c_str(), 320, 123);
	try
	{
		drawSdJpegCentered(secondTipeFileDirection.c_str(), 390, 123);
	}
	catch (const std::exception &e)
	{
		Serial.println("No second type");
	}
	drawSdJpegCentered(footPrintFileDirection.c_str(), 225, 163);
	SetAudioToPlay(audioFileDirection.c_str());
}

void DrawPokedexEntryScreen()
{
	drawSdJpeg("/pokedexEntry.jpg", 0, 0);
}

void drawTextInBox(String text, int x, int y, int box_width, int box_height)
{
	int cursor_x = x + 5; // Start with some padding from the box edges
	int cursor_y = y + 5;
	int line_height = 20; // Line height (adjust based on font size)

	String line = ""; // Buffer to build each line

	for (int i = 0; i < text.length(); i++)
	{
		char c = text[i];
		line += c; // Add character to the current line

		// Check the width of the current line
		int line_width = tft.textWidth(line);

		if (line_width >= box_width - 10 || c == '\n')
		{												 // Wrap if line is too wide or newline character
			tft.drawString(line, cursor_x, cursor_y, 2); // Draw the current line
			cursor_y += line_height;					 // Move to the next line
			line = "";									 // Reset line buffer

			// Check if we're outside the box vertically
			if (cursor_y > y + box_height - line_height)
			{
				break; // Stop if we exceed the box height
			}
		}
	}

	// Draw any remaining text in the line buffer
	if (line.length() > 0 && cursor_y <= y + box_height - line_height)
	{
		tft.drawString(line, cursor_x, cursor_y, 2);
	}
}

//? Audio Functions

void SetAudioToPlay(const char *filename)
{
	InputManager::canReadInput = false;
	if (!audio.connecttoFS(SD, filename))
	{ // Replace with your audio file's name
		Serial.println("Failed to open file for playback!");
	}
	else
	{
		Serial.println("Playing audio...");
	}
}

void audio_eof_mp3(const char *info)
{
	Serial.print("End of file reached: ");
	Serial.println(info);
	InputManager::canReadInput = true;
}

//% Image Displayer -----------------------------------------------------------------------------------------------------------------------------------------------
// Code obtained from https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Generic/ESP32_SDcard_jpeg/ESP32_SDcard_jpeg.ino

// ####################################################################################################
//  Draw a JPEG on the TFT pulled from SD Card
// ####################################################################################################
//  xpos, ypos is top left corner of plotted image
void drawSdJpeg(const char *filename, int xpos, int ypos)
{
	// Open the named file (the Jpeg decoder library will close it)
	File jpegFile = SD.open(filename, FILE_READ); // or, file handle reference for SD library

	if (!jpegFile)
	{
		Serial.print("ERROR: File \"");
		Serial.print(filename);
		Serial.println("\" not found!");
		return;
	}

	Serial.println("===========================");
	Serial.print("Drawing file: ");
	Serial.println(filename);
	Serial.println("===========================");

	// Use one of the following methods to initialise the decoder:
	bool decoded = JpegDec.decodeSdFile(jpegFile); // Pass the SD file handle to the decoder,
	// bool decoded = JpegDec.decodeSdFile(filename);  // or pass the filename (String or character array)

	if (decoded)
	{
		// print information about the image to the serial port
		// jpegInfo();
		// render the image onto the screen at given coordinates
		jpegRender(xpos, ypos);
	}
	else
	{
		Serial.println("Jpeg file format not supported!");
	}
}

void drawSdJpegCentered(const char *filename, int x_center, int y_center)
{
	// Open the named file (the Jpeg decoder library will close it)
	File jpegFile = SD.open(filename, FILE_READ);
	if (!jpegFile)
	{
		Serial.print("ERROR: File \"");
		Serial.print(filename);
		Serial.println("\" not found!");
		return;
	}

	Serial.println("===========================");
	Serial.print("Drawing file: ");
	Serial.println(filename);
	Serial.println("===========================");

	// Decode the JPEG file
	bool decoded = JpegDec.decodeSdFile(jpegFile);
	if (decoded)
	{
		// Calculate the top-left corner to center the image
		int img_width = JpegDec.width;
		int img_height = JpegDec.height;
		int xpos = x_center - img_width / 2;
		int ypos = y_center - img_height / 2;

		// Render the image onto the screen at the calculated coordinates
		jpegRender(xpos, ypos);
	}
	else
	{
		Serial.println("Jpeg file format not supported!");
	}
	jpegFile.close();
}

// ####################################################################################################
//  Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
// ####################################################################################################
//  This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
//  fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void jpegRender(int xpos, int ypos)
{

	// jpegInfo(); // Print information from the JPEG file (could comment this line out)

	uint16_t *pImg;
	uint16_t mcu_w = JpegDec.MCUWidth;
	uint16_t mcu_h = JpegDec.MCUHeight;
	uint32_t max_x = JpegDec.width;
	uint32_t max_y = JpegDec.height;

	bool swapBytes = tft.getSwapBytes();
	tft.setSwapBytes(true);

	// Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
	// Typically these MCUs are 16x16 pixel blocks
	// Determine the width and height of the right and bottom edge image blocks
	uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
	uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

	// save the current image block size
	uint32_t win_w = mcu_w;
	uint32_t win_h = mcu_h;

	// record the current time so we can measure how long it takes to draw an image
	uint32_t drawTime = millis();

	// save the coordinate of the right and bottom edges to assist image cropping
	// to the screen size
	max_x += xpos;
	max_y += ypos;

	// Fetch data from the file, decode and display
	while (JpegDec.read())
	{						   // While there is more data in the file
		pImg = JpegDec.pImage; // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

		// Calculate coordinates of top left corner of current MCU
		int mcu_x = JpegDec.MCUx * mcu_w + xpos;
		int mcu_y = JpegDec.MCUy * mcu_h + ypos;

		// check if the image block size needs to be changed for the right edge
		if (mcu_x + mcu_w <= max_x)
			win_w = mcu_w;
		else
			win_w = min_w;

		// check if the image block size needs to be changed for the bottom edge
		if (mcu_y + mcu_h <= max_y)
			win_h = mcu_h;
		else
			win_h = min_h;

		// copy pixels into a contiguous block
		if (win_w != mcu_w)
		{
			uint16_t *cImg;
			int p = 0;
			cImg = pImg + win_w;
			for (int h = 1; h < win_h; h++)
			{
				p += mcu_w;
				for (int w = 0; w < win_w; w++)
				{
					*cImg = *(pImg + w + p);
					cImg++;
				}
			}
		}

		// calculate how many pixels must be drawn
		uint32_t mcu_pixels = win_w * win_h;

		// draw image MCU block only if it will fit on the screen
		if ((mcu_x + win_w) <= tft.width() && (mcu_y + win_h) <= tft.height())
			tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
		else if ((mcu_y + win_h) >= tft.height())
			JpegDec.abort(); // Image has run off bottom of screen so abort decoding
	}

	tft.setSwapBytes(swapBytes);

	// showTime(millis() - drawTime); // These lines are for sketch testing only
}

// ####################################################################################################
//  Print image information to the serial port (optional)
// ####################################################################################################
//  JpegDec.decodeFile(...) or JpegDec.decodeArray(...) must be called before this info is available!
void jpegInfo()
{

	// Print information extracted from the JPEG file
	Serial.println("JPEG image info");
	Serial.println("===============");
	Serial.print("Width      :");
	Serial.println(JpegDec.width);
	Serial.print("Height     :");
	Serial.println(JpegDec.height);
	Serial.print("Components :");
	Serial.println(JpegDec.comps);
	Serial.print("MCU / row  :");
	Serial.println(JpegDec.MCUSPerRow);
	Serial.print("MCU / col  :");
	Serial.println(JpegDec.MCUSPerCol);
	Serial.print("Scan type  :");
	Serial.println(JpegDec.scanType);
	Serial.print("MCU width  :");
	Serial.println(JpegDec.MCUWidth);
	Serial.print("MCU height :");
	Serial.println(JpegDec.MCUHeight);
	Serial.println("===============");
	Serial.println("");
}

// ####################################################################################################
//  Show the execution time (optional)
// ####################################################################################################
//  WARNING: for UNO/AVR legacy reasons printing text to the screen with the Mega might not work for
//  sketch sizes greater than ~70KBytes because 16-bit address pointers are used in some libraries.

// The Due will work fine with the HX8357_Due library.

void showTime(uint32_t msTime)
{
	// tft.setCursor(0, 0);
	// tft.setTextFont(1);
	// tft.setTextSize(2);
	// tft.setTextColor(TFT_WHITE, TFT_BLACK);
	// tft.print(F(" JPEG drawn in "));
	// tft.print(msTime);
	// tft.println(F(" ms "));
	Serial.print(F(" JPEG drawn in "));
	Serial.print(msTime);
	Serial.println(F(" ms "));
}
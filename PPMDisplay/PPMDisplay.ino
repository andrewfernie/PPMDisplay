// -------------------------------------------------------------------------------------------
//
// Copyright (c) 2013 Andrew Fernie. All rights reserved.
//	Uses code derived from "Read-any-PPM" https://code.google.com/p/read-any-ppm/ArduPilotMega
//  released under GNU GPL v3. This code is released under the same conditions.
//  Uses Arduino LCD + Keypad Library "DFR_LCD_Keypad"	by Andy Gock (http://micro.gock.net)
//
// -------------------------------------------------------------------------------------------
//
// PPMDisplay
//
//   Designed for use on an Arduino with the common LCDKeypad display. Displays the RC servo pulse
//   length from the receiver. If input is a PPM signal (multiple pulses on a single line) then it
//   will capture and display up to 16 pulses.
//
//   Input is assumed to be on Arduino pin 3.
//
//   Use the Up and Down buttons to scroll through the channels
//
//   Use the Left button to select a display of the number of channels captured, frame update rate,
//   and two channels. Use the Right button to select display of four channels.
//
//   Use the Select button to reset the display.
//
//   It will also allow you to send a value out to a servo connected to the pin defined by
//   SERVO_OUT_PIN. You can send a minimum 1ms pulse, a center 1.5ms, or a maximum 2ms.
//   If you connect a pot to the pin defined by POT_PIN, such that it sends a voltage between
//   0V and 5V, then you can turn the pot to set the servo position, or use it to set the amplitude
//   around 1.5ms as the servo is swept back and forth.
//
//   You get to the servo output mode by pushing the right button twice, and select the output mode
//   with the up and down buttons.
//
// -------------------------------------------------------------------------------------------
#include <LiquidCrystal.h>
#include <DFR_LCD_Keypad.h>
#include <Keypad.h>
#include <Servo.h>
#include "PPMDisplay.h"

// You need to define which type of LCD and keypad you are using. Easiest approach is the typical Arduino LCD Keypad shield.
// If that is what you have, set both KEYPAD_TYPE and DISPLAY_TYPE to LCDKEYPAD_SHIELD by uncommenting the two lines below (and
// making sure that the lines below them are commented out.
// Use these lines when using the typical Arduino LCD Keypad shield
//
//#define KEYPAD_TYPE  LCDKEYPAD_SHIELD
//#define DISPLAY_TYPE LCDKEYPAD_SHIELD

// Use these lines for a home-built keypad and an I2C LCD display. This is useful as you can purchase a case that the 16x2 line
// LCD modules fit into and then you make your own keypad using the same type of resistor network as used on the LCD Keypad Shield.
// Search for "Arduino LCD Keypad Shield Schematic" with Google and you will find what you need.
//
#define KEYPAD_TYPE  PERSONAL_KEYPAD
#define DISPLAY_TYPE I2C_DISPLAY

// The hassle is you make your own keypad is that you may not have the same resistors as in the schematic, in which case the
// ADC values read by the program to determine which key has been pressed will be off. Don't worry - just uncomment the following
// line and the LCD screen will show the value read by the ADC as you press each key. The values need to be entered in the code
// further below (where the "#define KEYPAD_KEY_RIGHT_ADC_LOW" line starts.
//
//#define TEST_KEY_ADC_VALUES			// uncomment to test your key ADC values


#define CHAN_NUM_DISPLAY_START 1               // Set to 0 to start channel number display at 0, or 1 to start it at 1

#if DISPLAY_TYPE==I2C_DISPLAY
#include <Wire.h>

#define _Digole_Serial_I2C_  //To tell compiler compile the special communication only, 

#include <DigoleSerial.h>

DigoleSerialDisp LCDDisplay_I2C(&Wire, '\x27'); //I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#else if DISPLAYTYPE == LCDKEYPAD_SHIELD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#endif

#define PPM_Pin 3  //this must be 2 or 3
#define NUM_CHANNELS 16
int ppm[NUM_CHANNELS];  //array for storing up to 16 servo signals
long frameTime;
double frameRate;

#define	KEYPAD_PIN A0

// initialize the keypad
#if KEYPAD_TYPE == LCDKEYPAD_SHIELD
DFR_LCD_Keypad keypad(KEYPAD_PIN);
#else if KEYPAD_TYPE == PERSONAL_KEYPAD

// These values need to be measured for your particular resistor divider network. Add a "print" statement somewhere in the main loop to send the value of the KEYPAD_PIN analog input
// to the terminal (or an LCDprint statement to send it to the LCD). Record trhe values from each button press, and then add/subtract a tolerance (around 10-20) to get the HIGH and LOW
// values shown below.
//
// No need to worry about this if you use a standard Arduino LCD Keypad shield. The values are initialized automatically in the Keypad class constructor.
//
#define KEYPAD_KEY_RIGHT_ADC_LOW   0
#define KEYPAD_KEY_RIGHT_ADC_HIGH  20
#define KEYPAD_KEY_UP_ADC_LOW      254
#define KEYPAD_KEY_UP_ADC_HIGH     294
#define KEYPAD_KEY_DOWN_ADC_LOW    395
#define KEYPAD_KEY_DOWN_ADC_HIGH   435
#define KEYPAD_KEY_LEFT_ADC_LOW    524
#define KEYPAD_KEY_LEFT_ADC_HIGH   564
#define KEYPAD_KEY_SELECT_ADC_LOW  720
#define KEYPAD_KEY_SELECT_ADC_HIGH 760
#define KEYPAD_KEY_NONE_ADC_LOW    1000
#define KEYPAD_KEY_NONE_ADC_HIGH   1023

uint16_t myKeypadValues[12] = {	 KEYPAD_KEY_RIGHT_ADC_LOW,   KEYPAD_KEY_RIGHT_ADC_HIGH,
                                 KEYPAD_KEY_UP_ADC_LOW,      KEYPAD_KEY_UP_ADC_HIGH,
                                 KEYPAD_KEY_DOWN_ADC_LOW, 	 KEYPAD_KEY_DOWN_ADC_HIGH,
                                 KEYPAD_KEY_LEFT_ADC_LOW,    KEYPAD_KEY_LEFT_ADC_HIGH,
                                 KEYPAD_KEY_SELECT_ADC_LOW,  KEYPAD_KEY_SELECT_ADC_HIGH,
                                 KEYPAD_KEY_NONE_ADC_LOW,    KEYPAD_KEY_NONE_ADC_HIGH
                              };

uint16_t myKeypadValue = 0;

//	Keypad keypad(KEYPAD_PIN, myKeypadValues);
Keypad keypad(KEYPAD_PIN);

#endif

int last_key, key;
int channelDisplay;
int showStats;
int numChannels;

int displayMode = DISPLAYMODE_READSTATS;
int oldDisplayMode = -1;

#define SERVO_OUT_PIN 5							// Pin used for the the servo drive
#define POT_PIN A1					// Analog input pin used to read the potentiometer used to set servo out position.

Servo servoOut;  // create servo object to control a servo
int servoOutMode = SERVOOUT_CENTER;
int oldServoOutMode;
int servoOutValue;

int servoOutCycleMax = SERVOOUT_MAX_VALUE;
int servoOutCycleMin = SERVOOUT_MIN_VALUE;
int servoOutCycleDisplayBand = 0;

int lastFastIterationStart = 0;
int lastSlowIterationStart = 0;
int lastSuperSlowIterationStart = 0;

void read_ppm();
void reset_display();
void EnablePPMInterrupt();
void DisablePPMInterrupt();
void LCDSetPrintPos(uint8_t x, uint8_t y);
void LCDPrint(const char v[]);
void LCDClearScreen();

void setup()
{
    Serial.begin(115200);
    Serial.println("ready");

#if DISPLAY_TYPE == LCDKEYPAD_SHIELD
    // Initialize a parallel drive display like on the LCD Keypad Shield
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
#else if DISPLAY_TYPE == I2C_DISPLAY
    // Initialize I2C LCD Display
    LCDDisplay_I2C.begin();
    LCDDisplay_I2C.disableCursor(); //disable cursor
#endif

#if KEYPAD_TYPE == PERSONAL_KEYPAD
    keypad.SetKeyValues(myKeypadValues);
#endif

    pinMode(PPM_Pin, INPUT);

    reset_display();
}

void loop()
{
    int count;
    char lcdLine[17];
    int thisIterationStart;
    int potValue;
    int cyclePhaseInt;
    float cyclePhase;

    thisIterationStart = millis();

    if ((thisIterationStart - lastFastIterationStart) > FAST_ITERATION_MILLIS)
    {

        potValue = analogRead(POT_PIN);		// will be 0-1023

        if (displayMode)
        {
            switch (servoOutMode)
            {
            case SERVOOUT_CENTER:
                servoOutValue = SERVOOUT_CENTER_VALUE;
                break;

            case SERVOOUT_MINPPM:
                servoOutValue = SERVOOUT_MIN_VALUE;
                break;

            case SERVOOUT_MAXPPM:
                servoOutValue = SERVOOUT_MAX_VALUE;
                break;

            case SERVOOUT_CYCLE:
                cyclePhaseInt = (thisIterationStart % SERVOOUT_CYCLE_MILLIS);
                cyclePhase = (float)(cyclePhaseInt) / SERVOOUT_CYCLE_MILLIS * 2 * PI;
                servoOutValue = min(max(SERVOOUT_MIN_VALUE, sin(cyclePhase) * (potValue) / 1023.0 * 500.0 + SERVOOUT_CENTER_VALUE), SERVOOUT_MAX_VALUE);

                break;

            case SERVOOUT_VARIABLE:
                servoOutValue = min(max(SERVOOUT_MIN_VALUE, (potValue - 512) + SERVOOUT_CENTER_VALUE), SERVOOUT_MAX_VALUE);
                break;
            }

            servoOut.writeMicroseconds(servoOutValue);                  // sets the servo position according to the scaled value
        }
        lastFastIterationStart = thisIterationStart;
    }

    thisIterationStart = millis();
    if ((thisIterationStart - lastSlowIterationStart) > SLOW_ITERATION_MILLIS)
    {

        last_key = keypad.get_last_key();
        key      = keypad.read_key();

        if (key != last_key)
        {
            // key has changed

            switch (key)
            {
            case KEY_RIGHT:
                displayMode = min(displayMode + 1, DISPLAYMODE_MAX);
                break;

            case KEY_UP:
                if ((displayMode == DISPLAYMODE_READ4CHAN) || (displayMode == DISPLAYMODE_READSTATS))
                {
                    if (channelDisplay <= 2)
                    {
                        channelDisplay = 0;
                    }
                    else
                    {
                        channelDisplay = channelDisplay - 2;
                    }

                    LCDClearScreen();
                }
                else if (displayMode == DISPLAYMODE_SERVOOUT)
                {
                    servoOutMode = min(servoOutMode + 1, SERVOOUT_MAX);
                    LCDClearScreen();
                }
                break;

            case KEY_DOWN:
                if (displayMode == DISPLAYMODE_READSTATS)
                {
                    if (channelDisplay >= NUM_CHANNELS - 2)
                    {
                        channelDisplay = NUM_CHANNELS - 2;
                    }
                    else
                    {
                        channelDisplay = channelDisplay + 2;
                    }
                    LCDClearScreen();
                }
                else if (displayMode == DISPLAYMODE_READ4CHAN)
                {
                    if (channelDisplay >= NUM_CHANNELS - 4)
                    {
                        channelDisplay = NUM_CHANNELS - 4;
                    }
                    else
                    {
                        channelDisplay = channelDisplay + 2;
                    }
                    LCDClearScreen();
                }
                else if (displayMode == DISPLAYMODE_SERVOOUT)
                {
                    servoOutMode = max(0, servoOutMode - 1);
                    LCDClearScreen();
                }
                break;

            case KEY_LEFT:
                displayMode = max(0, displayMode - 1);
                break;

            case KEY_SELECT:
                reset_display();
                break;

            case KEY_NONE:
            default:
                break;
            }
        }

        switch (displayMode)
        {
        case DISPLAYMODE_READSTATS:
            if (displayMode != oldDisplayMode)
            {
                EnablePPMInterrupt();
                LCDClearScreen();
            }

            if (frameTime > 0)
            {
                frameRate = 1000000.0 / (double)frameTime;
            }
            else
            {
                frameRate = 0.0;
            }

#ifndef TEST_KEY_ADC_VALUES
            LCDSetPrintPos(0, 0);

            sprintf(lcdLine, "NumCh:%2d ", numChannels);
            LCDPrint(lcdLine);

            dtostrf(frameRate, 5, 1, lcdLine);
            LCDPrint(lcdLine);

            LCDPrint("Hz");
#endif

            LCDSetPrintPos(0, 1);
            sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay + CHAN_NUM_DISPLAY_START, ppm[channelDisplay], 
				                                channelDisplay + 1 + CHAN_NUM_DISPLAY_START, ppm[channelDisplay + 1]);
            LCDPrint(lcdLine);
            break;

        case DISPLAYMODE_READ4CHAN:
            if (displayMode != oldDisplayMode)
            {
                EnablePPMInterrupt();

                channelDisplay = min(channelDisplay, NUM_CHANNELS - 4);
                LCDClearScreen();
            }

#ifndef TEST_KEY_ADC_VALUES
            LCDSetPrintPos(0, 0);
            sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay + CHAN_NUM_DISPLAY_START, ppm[channelDisplay], 
				                                channelDisplay + 1 + CHAN_NUM_DISPLAY_START, ppm[channelDisplay + 1]);
            LCDPrint(lcdLine);
#endif

            LCDSetPrintPos(0, 1);
            sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay + 2 + CHAN_NUM_DISPLAY_START, ppm[channelDisplay + 2], 
				                                channelDisplay + 3 + CHAN_NUM_DISPLAY_START, ppm[channelDisplay + 3]);
            LCDPrint(lcdLine);

            break;

        case DISPLAYMODE_SERVOOUT:
            if (displayMode != oldDisplayMode)
            {
                DisablePPMInterrupt();
                servoOut.attach(SERVO_OUT_PIN);
                LCDClearScreen();
            }

#ifndef TEST_KEY_ADC_VALUES
            LCDSetPrintPos(0, 0);
            sprintf(lcdLine, "PPM OUT");
            LCDPrint(lcdLine);

            switch (servoOutMode)
            {
            case SERVOOUT_CENTER:
                sprintf(lcdLine, "  CENTER");
                LCDPrint(lcdLine);
                break;

            case SERVOOUT_MINPPM:
                sprintf(lcdLine, "  MIN   ");
                LCDPrint(lcdLine);
                break;

            case SERVOOUT_MAXPPM:
                sprintf(lcdLine, "  MAX   ");
                LCDPrint(lcdLine);
                break;

            case SERVOOUT_CYCLE:
                sprintf(lcdLine, "  CYCLE  ");
                LCDPrint(lcdLine);

                if (servoOutCycleDisplayBand == 2)  				// need to subband as the data changes too quickly to read on the display
                {
                    LCDSetPrintPos(0, 1);
                    sprintf(lcdLine, "%d us", servoOutValue);		// value won't be valid until next cycle
                    LCDPrint(lcdLine);
                    servoOutCycleDisplayBand = 0;
                }
                else
                {
                    servoOutCycleDisplayBand++;
                }
                break;

            case SERVOOUT_VARIABLE:
                sprintf(lcdLine, " VARIABLE");
                LCDPrint(lcdLine);

                LCDSetPrintPos(0, 1);
                sprintf(lcdLine, "%d us", servoOutValue);		// value won't be valid until next cycle
                LCDPrint(lcdLine);
                break;
            }
#endif
            break;
        }


        // Reset data - will be filled in before delay expires
        numChannels = 0;
        frameTime = 0;
        for (int i = 0; i < NUM_CHANNELS; i++)
        {
            ppm[i] = 0;
        }


#ifdef TEST_KEY_ADC_VALUES
        LCDSetPrintPos(0, 0);
        sprintf(lcdLine, "A0:%4d ", analogRead(A0));			// Uncomment if you want to check the pot values
        LCDPrint(lcdLine);
#endif

        oldDisplayMode = displayMode;

        lastSlowIterationStart = thisIterationStart;
    }

    thisIterationStart = millis();
    if ((thisIterationStart - lastSuperSlowIterationStart) > SUPERSLOW_ITERATION_MILLIS)
    {
        lastSuperSlowIterationStart = thisIterationStart;
    }
}



// PPM Input Interrupt Handler
void read_ppm()
{
    static unsigned int pulse;
    static unsigned long counter;

    static unsigned long frameStartCount;
    static unsigned long tmpl;
    static byte channel;

    counter = TCNT1;
    TCNT1 = 0;

    if (counter < 1020)   //must be a leading negative pulse that adds to a PPM positive pulse if less than 510us
    {
        pulse = counter;
    }
    else if (counter > 4500)     //sync pulses over 2250us
    {
        channel = 0;
        pulse = 0;
    }
    else     //servo values between 510us and 2250us will end up here
    {
        if (channel < NUM_CHANNELS)
        {
            ppm[channel] = (counter + pulse) / 2;
        }

        if (channel == 0)
        {
            tmpl = micros();
            frameTime = (tmpl - frameStartCount);
            frameStartCount = tmpl;
        }

        channel++;
        numChannels = max(channel, numChannels);
    }
}

void reset_display()
{
    channelDisplay = 0;
    showStats = true;
    numChannels = 0;

    LCDClearScreen(); //Clear screen
}


void LCDSetPrintPos(uint8_t x, uint8_t y)
{
#if DISPLAY_TYPE == LCDKEYPAD_SHIELD
    lcd.setCursor(x, y);
#else if DISPLAY_TYPE == I2C_DISPLAY
    LCDDisplay_I2C.setPrintPos(x, y);
#endif
}

void LCDPrint(const char v[])
{
#if DISPLAY_TYPE == LCDKEYPAD_SHIELD
    lcd.print(v);
#else if DISPLAY_TYPE == I2C_DISPLAY
    LCDDisplay_I2C.print(v);
#endif
}

void LCDClearScreen()
{
#if DISPLAY_TYPE == LCDKEYPAD_SHIELD
    lcd.clear();
#else if DISPLAY_TYPE == I2C_DISPLAY
    LCDDisplay_I2C.clearScreen();
#endif
}


void EnablePPMInterrupt()
{
    pinMode(PPM_Pin, INPUT);

    attachInterrupt(PPM_Pin - 2, read_ppm, CHANGE);

    TCCR1A = 0;  //reset timer1
    TCCR1B = 0;
    TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us
}

void DisablePPMInterrupt()
{
    detachInterrupt(PPM_Pin - 2);
}

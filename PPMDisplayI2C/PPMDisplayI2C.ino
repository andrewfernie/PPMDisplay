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
// -------------------------------------------------------------------------------------------

#define MY_KEYPAD
#include <LiquidCrystal.h>
#include <DFR_LCD_Keypad.h>
#include <Wire.h>

#define PPM_Pin 3  //this must be 2 or 3
#define NUM_CHANNELS 16
int ppm[NUM_CHANNELS];  //array for storing up to 16 servo signals
long frameTime;
double frameRate;

#define _Digole_Serial_I2C_  //To tell compiler compile the special communication only, 
#include <DigoleSerial.h>

DigoleSerialDisp LCDDisplay_I2C(&Wire,'\x27');  //I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#define LCDCol 16
#define LCDRow 2

// initialize the keypad
DFR_LCD_Keypad keypad(A0);
int last_key, key;
int channelDisplay;
int showStats;
int numChannels;

void setup()
{
    Serial.begin(115200);
    Serial.println("ready");
   
	// Initialize I2C LCD Display
	LCDDisplay_I2C.begin();
	LCDDisplay_I2C.clearScreen(); //CLear screen
    LCDDisplay_I2C.disableCursor(); //disable cursor

    pinMode(PPM_Pin, INPUT);
    attachInterrupt(PPM_Pin - 2, read_ppm, CHANGE);

    TCCR1A = 0;  //reset timer1
    TCCR1B = 0;
    TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us

    reset_display();
}

void loop()
{
    int count;
    char lcdLine[17];

    last_key = keypad.get_last_key();
    key      = keypad.read_key();

    if (key != last_key) {
        // key has changed

        switch (key) {
        case KEY_RIGHT:
            showStats = false;
            channelDisplay = min(channelDisplay, NUM_CHANNELS - 4);
            LCDDisplay_I2C.clearScreen();
            break;

        case KEY_UP:
            if (channelDisplay <= 2) {
                channelDisplay = 0;
            } else {
                channelDisplay = channelDisplay - 2;
            }

            LCDDisplay_I2C.clearScreen();
            break;

        case KEY_DOWN:
            if (showStats) {
                if (channelDisplay >= NUM_CHANNELS - 2) {
                    channelDisplay = NUM_CHANNELS - 2;
                } else {
                    channelDisplay = channelDisplay + 2;
                }
            } else {
                if (channelDisplay >= NUM_CHANNELS - 4) {
                    channelDisplay = NUM_CHANNELS - 4;
                } else {
                    channelDisplay = channelDisplay + 2;
                }
            }
            LCDDisplay_I2C.clearScreen();
            break;

        case KEY_LEFT:
            showStats = true;
            LCDDisplay_I2C.clearScreen();
            break;

        case KEY_SELECT:
            reset_display();
            break;

        case KEY_NONE:
        default:
            break;
        }
    }

    if (showStats) {
        LCDDisplay_I2C.setPrintPos(0, 0);

        sprintf(lcdLine, "NumCh:%2d ", numChannels);
 //       sprintf(lcdLine, "A0:%4d ", analogRead(A0));
        LCDDisplay_I2C.print(lcdLine);
        if (frameTime > 0) {
            frameRate = 1000000.0 / (double)frameTime;
        } else {
            frameRate = 0.0;
        }

        dtostrf(frameRate, 5, 1, lcdLine);
        LCDDisplay_I2C.print(lcdLine);

        LCDDisplay_I2C.print("Hz");

        LCDDisplay_I2C.setPrintPos(0, 1);
        sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay, ppm[channelDisplay], channelDisplay + 1, ppm[channelDisplay + 1]);
        LCDDisplay_I2C.print(lcdLine);

    } else {
        LCDDisplay_I2C.setPrintPos(0, 0);
        sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay, ppm[channelDisplay], channelDisplay + 1, ppm[channelDisplay + 1]);
        LCDDisplay_I2C.print(lcdLine);

        LCDDisplay_I2C.setPrintPos(0, 1);
        sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay + 2, ppm[channelDisplay + 2], channelDisplay + 3, ppm[channelDisplay + 3]);
        LCDDisplay_I2C.print(lcdLine);

    }

    // Reset data - will be filled in before delay expires
    numChannels = 0;
    frameTime = 0;
    for (int i = 0; i < NUM_CHANNELS; i++) {
        ppm[i] = 0;
    }

    delay(200);
}



void read_ppm()   //leave this alone
{
    static unsigned int pulse;
    static unsigned long counter;

    static unsigned long frameStartCount;
    static unsigned long tmpl;
    static byte channel;

    counter = TCNT1;
    TCNT1 = 0;

    if (counter < 1020) { //must be a pulse if less than 510us
        pulse = counter;
    } else if (counter > 4500) { //sync pulses over 2250us
        channel = 0;
    } else { //servo values between 510us and 2250us will end up here

        if (channel < NUM_CHANNELS) {
            ppm[channel] = (counter + pulse) / 2;
        }

        if (channel == 0) {
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
    LCDDisplay_I2C.clearScreen();
}
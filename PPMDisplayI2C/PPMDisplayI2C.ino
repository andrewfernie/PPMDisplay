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
#include <Servo.h>

#define PPM_Pin 3  //this must be 2 or 3
#define NUM_CHANNELS 16
int ppm[NUM_CHANNELS];  //array for storing up to 16 servo signals
long frameTime;
double frameRate;

#define _Digole_Serial_I2C_  //To tell compiler compile the special communication only, 
#include <DigoleSerial.h>

DigoleSerialDisp LCDDisplay_I2C(&Wire, '\x27'); //I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#define LCDCol 16
#define LCDRow 2

// initialize the keypad
DFR_LCD_Keypad keypad(A0);
int last_key, key;
int channelDisplay;
int showStats;
int numChannels;

#define DISPLAYMODE_READSTATS 0
#define DISPLAYMODE_READ4CHAN 1
#define DISPLAYMODE_SERVOOUT 2
#define DISPLAYMODE_MAX 2

#define SERVOOUT_CENTER 0
#define SERVOOUT_MINPPM 1
#define SERVOOUT_MAXPPM 2
#define SERVOOUT_CYCLE 3
#define SERVOOUT_VARIABLE 4
#define SERVOOUT_MAX 4

#define SERVOOUT_MAX_VALUE 2000
#define SERVOOUT_MIN_VALUE 1000
#define SERVOOUT_CENTER_VALUE 1500
#define SERVOOUT_CYCLE_MILLIS 4000     // Number of milliseconds for one cycle of the servo output CYCLE mode      



int displayMode = DISPLAYMODE_READSTATS;
int oldDisplayMode = -1;

Servo servoOut;  // create servo object to control a servo
int servoOutMode = SERVOOUT_CENTER;
int oldServoOutMode;
int servoOutValue;

int servoOutCycleMax = SERVOOUT_MAX_VALUE;
int servoOutCycleMin = SERVOOUT_MIN_VALUE;
int servoOutCycleDisplayBand = 0;


#define FAST_ITERATION_MILLIS 20
#define SLOW_ITERATION_MILLIS 100

int lastFastIterationStart = 0;
int lastSlowIterationStart = 0;

#define POT_PIN 1


void setup()
{
    Serial.begin(115200);
    Serial.println("ready");

    // Initialize I2C LCD Display
    LCDDisplay_I2C.begin();
    LCDDisplay_I2C.clearScreen(); //CLear screen
    LCDDisplay_I2C.disableCursor(); //disable cursor

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

    if ((thisIterationStart - lastFastIterationStart) > FAST_ITERATION_MILLIS) {

        potValue = analogRead(POT_PIN);		// will be 0-1023

        if (displayMode) {
            switch (servoOutMode) {
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

    if ((thisIterationStart - lastSlowIterationStart) > SLOW_ITERATION_MILLIS) {

        last_key = keypad.get_last_key();
        key      = keypad.read_key();

        if (key != last_key) {
            // key has changed

            switch (key) {
            case KEY_RIGHT:
                displayMode = min(displayMode + 1, DISPLAYMODE_MAX);

                break;

            case KEY_UP:
                if ((displayMode == DISPLAYMODE_READ4CHAN) || (displayMode == DISPLAYMODE_READSTATS)) {
                    if (channelDisplay <= 2) {
                        channelDisplay = 0;
                    } else {
                        channelDisplay = channelDisplay - 2;
                    }

                    LCDDisplay_I2C.clearScreen();
                } else if (displayMode == DISPLAYMODE_SERVOOUT) {
                    servoOutMode = min(servoOutMode + 1, SERVOOUT_MAX);

                }
                break;

            case KEY_DOWN:
                if (displayMode == DISPLAYMODE_READSTATS) {
                    if (channelDisplay >= NUM_CHANNELS - 2) {
                        channelDisplay = NUM_CHANNELS - 2;
                    } else {
                        channelDisplay = channelDisplay + 2;
                    }
                    LCDDisplay_I2C.clearScreen();
                } else if (displayMode == DISPLAYMODE_READ4CHAN) {
                    if (channelDisplay >= NUM_CHANNELS - 4) {
                        channelDisplay = NUM_CHANNELS - 4;
                    } else {
                        channelDisplay = channelDisplay + 2;
                    }
                    LCDDisplay_I2C.clearScreen();
                } else if (displayMode == DISPLAYMODE_SERVOOUT) {
                    servoOutMode = max(0, servoOutMode - 1);

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

        switch (displayMode) {
        case DISPLAYMODE_READSTATS:
            if (displayMode != oldDisplayMode) {
                pinMode(PPM_Pin, INPUT);
                attachInterrupt(PPM_Pin - 2, read_ppm, CHANGE);

                TCCR1A = 0;  //reset timer1
                TCCR1B = 0;
                TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us

                LCDDisplay_I2C.clearScreen();
            }
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
            break;

        case DISPLAYMODE_READ4CHAN:
            if (displayMode != oldDisplayMode) {
                pinMode(PPM_Pin, INPUT);
                attachInterrupt(PPM_Pin - 2, read_ppm, CHANGE);

                TCCR1A = 0;  //reset timer1
                TCCR1B = 0;
                TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us

                channelDisplay = min(channelDisplay, NUM_CHANNELS - 4);
                LCDDisplay_I2C.clearScreen();
            }

            LCDDisplay_I2C.setPrintPos(0, 0);
            sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay, ppm[channelDisplay], channelDisplay + 1, ppm[channelDisplay + 1]);
            LCDDisplay_I2C.print(lcdLine);

            LCDDisplay_I2C.setPrintPos(0, 1);
            sprintf(lcdLine, "%2d:%4d %2d:%4d", channelDisplay + 2, ppm[channelDisplay + 2], channelDisplay + 3, ppm[channelDisplay + 3]);
            LCDDisplay_I2C.print(lcdLine);

            break;

        case DISPLAYMODE_SERVOOUT:
            if (displayMode != oldDisplayMode) {
                detachInterrupt(PPM_Pin - 2);
                servoOut.attach(5);
                LCDDisplay_I2C.clearScreen();
            }

            LCDDisplay_I2C.setPrintPos(0, 0);
            sprintf(lcdLine, "PPM OUT");
            LCDDisplay_I2C.print(lcdLine);

            switch (servoOutMode) {
            case SERVOOUT_CENTER:
                sprintf(lcdLine, "  CENTER");
                LCDDisplay_I2C.print(lcdLine);
                break;

            case SERVOOUT_MINPPM:
                sprintf(lcdLine, "  MIN   ");
                LCDDisplay_I2C.print(lcdLine);
                break;

            case SERVOOUT_MAXPPM:
                sprintf(lcdLine, "  MAX   ");
                LCDDisplay_I2C.print(lcdLine);
                break;

            case SERVOOUT_CYCLE:
                sprintf(lcdLine, "  CYCLE  ");
                LCDDisplay_I2C.print(lcdLine);

                if (servoOutCycleDisplayBand == 2) {				// need to subband as the data changes too quickly to read on the display
                    LCDDisplay_I2C.setPrintPos(0, 1);
                    sprintf(lcdLine, "%d us", servoOutValue);		// value won't be valid until next cycle
                    LCDDisplay_I2C.print(lcdLine);
                    servoOutCycleDisplayBand = 0;
                } else {
                    servoOutCycleDisplayBand++;
                }
                break;

            case SERVOOUT_VARIABLE:
                sprintf(lcdLine, " VARIABLE");
                LCDDisplay_I2C.print(lcdLine);

                LCDDisplay_I2C.setPrintPos(0, 1);
                sprintf(lcdLine, "%d us", servoOutValue);		// value won't be valid until next cycle
                LCDDisplay_I2C.print(lcdLine);
                break;
            }

            break;

        }

        // Reset data - will be filled in before delay expires
        numChannels = 0;
        frameTime = 0;
        for (int i = 0; i < NUM_CHANNELS; i++) {
            ppm[i] = 0;
        }

        oldDisplayMode = displayMode;

        lastSlowIterationStart = thisIterationStart;
    }

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

    if (counter < 1020) { //must be a leading negative pulse that adds to a PPM positive pulse if less than 510us
        pulse = counter;
    } else if (counter > 4500) { //sync pulses over 2250us
        channel = 0;
		pulse = 0;
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
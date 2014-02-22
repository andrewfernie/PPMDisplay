/*
	Arduino LCD + Keypad Library
	by Andy Gock
	http://micro.gock.net

	For use with common LCD / Keypad shields such as:

	DFRobot DFR0009
	http://www.dfrobot.com/wiki/index.php?title=Arduino_LCD_KeyPad_Shield_%28SKU:_DFR0009%29

 */
#ifndef DFR_LCD_KEYPAD_H
#define DFR_LCD_KEYPAD_H

#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef LCD_CLEARDISPLAY
#include <LiquidCrystal.h>
#endif

#if !defined(DFR_LCD_KEYPAD_BACKLIGHT_PIN)
#define DFR_LCD_KEYPAD_BACKLIGHT_PIN 10
#endif

#if !defined(DFR_LCD_KEYPAD_KEY_PIN)
#define DFR_LCD_KEYPAD_KEY_PIN       A0
#endif

#define DFR_LCD_KEYPAD_KEY_RIGHT  0
#define DFR_LCD_KEYPAD_KEY_UP     1
#define DFR_LCD_KEYPAD_KEY_DOWN   2
#define DFR_LCD_KEYPAD_KEY_LEFT   3
#define DFR_LCD_KEYPAD_KEY_SELECT 4
#define DFR_LCD_KEYPAD_KEY_NONE   5

#define DFR_LCD_KEYPAD_KEY_RIGHT_ADC_LOW   0
#define DFR_LCD_KEYPAD_KEY_RIGHT_ADC_HIGH  20
#define DFR_LCD_KEYPAD_KEY_UP_ADC_LOW      120
#define DFR_LCD_KEYPAD_KEY_UP_ADC_HIGH     140
#define DFR_LCD_KEYPAD_KEY_DOWN_ADC_LOW    290
#define DFR_LCD_KEYPAD_KEY_DOWN_ADC_HIGH   339
#define DFR_LCD_KEYPAD_KEY_LEFT_ADC_LOW    450
#define DFR_LCD_KEYPAD_KEY_LEFT_ADC_HIGH   520
#define DFR_LCD_KEYPAD_KEY_SELECT_ADC_LOW  700
#define DFR_LCD_KEYPAD_KEY_SELECT_ADC_HIGH 780
#define DFR_LCD_KEYPAD_KEY_NONE_ADC_LOW    1000
#define DFR_LCD_KEYPAD_KEY_NONE_ADC_HIGH   1023

#define KEY_RIGHT  DFR_LCD_KEYPAD_KEY_RIGHT
#define KEY_UP     DFR_LCD_KEYPAD_KEY_UP
#define KEY_DOWN   DFR_LCD_KEYPAD_KEY_DOWN
#define KEY_LEFT   DFR_LCD_KEYPAD_KEY_LEFT
#define KEY_SELECT DFR_LCD_KEYPAD_KEY_SELECT
#define KEY_NONE   DFR_LCD_KEYPAD_KEY_NONE

class DFR_LCD_Keypad
{
private:
    uint8_t _key_pin;
    uint16_t _last_key;
    LiquidCrystal *_lcd;

public:
    DFR_LCD_Keypad();
    DFR_LCD_Keypad(const uint8_t key_pin);
    DFR_LCD_Keypad(const uint8_t key_pin, LiquidCrystal *lcd);
    void backlight_off(void);
    void backlight_on(void);
    uint8_t read_key(void);
    uint8_t get_last_key(void);
};

#endif

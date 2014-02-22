// Keypad.h

#ifndef _KEYPAD_h
#define _KEYPAD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#if !defined(KEYPAD_KEY_PIN)
#define KEYPAD_KEY_PIN       A0
#endif

#define KEYPAD_KEY_RIGHT  0
#define KEYPAD_KEY_UP     1
#define KEYPAD_KEY_DOWN   2
#define KEYPAD_KEY_LEFT   3
#define KEYPAD_KEY_SELECT 4
#define KEYPAD_KEY_NONE   5

#define LCD_KEYPAD_SHIELD_KEY_RIGHT_ADC_LOW   0
#define LCD_KEYPAD_SHIELD_KEY_RIGHT_ADC_HIGH  20
#define LCD_KEYPAD_SHIELD_KEY_UP_ADC_LOW      120
#define LCD_KEYPAD_SHIELD_KEY_UP_ADC_HIGH     140
#define LCD_KEYPAD_SHIELD_KEY_DOWN_ADC_LOW    290
#define LCD_KEYPAD_SHIELD_KEY_DOWN_ADC_HIGH   339
#define LCD_KEYPAD_SHIELD_KEY_LEFT_ADC_LOW    450
#define LCD_KEYPAD_SHIELD_KEY_LEFT_ADC_HIGH   520
#define LCD_KEYPAD_SHIELD_KEY_SELECT_ADC_LOW  700
#define LCD_KEYPAD_SHIELD_KEY_SELECT_ADC_HIGH 780
#define LCD_KEYPAD_SHIELD_KEY_NONE_ADC_LOW    1000
#define LCD_KEYPAD_SHIELD_KEY_NONE_ADC_HIGH   1023


#define KEY_RIGHT  KEYPAD_KEY_RIGHT
#define KEY_UP     KEYPAD_KEY_UP
#define KEY_DOWN   KEYPAD_KEY_DOWN
#define KEY_LEFT   KEYPAD_KEY_LEFT
#define KEY_SELECT KEYPAD_KEY_SELECT
#define KEY_NONE   KEYPAD_KEY_NONE

class Keypad
{
private:
    uint8_t _key_pin;
    uint16_t _last_key;
    uint16_t _keyADCValues[12];

public:
    Keypad();
    Keypad(const uint8_t key_pin);
    void SetKeyValues(uint16_t keyADCValues[12]);
    uint8_t read_key(void);
    uint8_t get_last_key(void);
};

#endif


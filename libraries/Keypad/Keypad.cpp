//
//
//

#include "Keypad.h"

Keypad::Keypad()
{
    Keypad::Keypad(KEYPAD_KEY_PIN);
}

Keypad::Keypad(const uint8_t key_pin)
{
    _key_pin = key_pin;
    _last_key = KEYPAD_KEY_NONE;

    _keyADCValues[0] = LCD_KEYPAD_SHIELD_KEY_RIGHT_ADC_LOW;
    _keyADCValues[1] = LCD_KEYPAD_SHIELD_KEY_RIGHT_ADC_HIGH;
    _keyADCValues[2] = LCD_KEYPAD_SHIELD_KEY_UP_ADC_LOW;
    _keyADCValues[3] = LCD_KEYPAD_SHIELD_KEY_UP_ADC_HIGH;
    _keyADCValues[4] = LCD_KEYPAD_SHIELD_KEY_DOWN_ADC_LOW;
    _keyADCValues[5] = LCD_KEYPAD_SHIELD_KEY_DOWN_ADC_HIGH;
    _keyADCValues[6] = LCD_KEYPAD_SHIELD_KEY_LEFT_ADC_LOW;
    _keyADCValues[7] = LCD_KEYPAD_SHIELD_KEY_LEFT_ADC_HIGH;
    _keyADCValues[8] = LCD_KEYPAD_SHIELD_KEY_SELECT_ADC_LOW;
    _keyADCValues[9] = LCD_KEYPAD_SHIELD_KEY_SELECT_ADC_HIGH;
    _keyADCValues[10] = LCD_KEYPAD_SHIELD_KEY_NONE_ADC_LOW;
    _keyADCValues[11] = LCD_KEYPAD_SHIELD_KEY_NONE_ADC_HIGH;
}

void Keypad::SetKeyValues(uint16_t keyADCValues[12])
{
    for (int i = 0; i < 12; i++) _keyADCValues[i] = keyADCValues[i];
}

uint8_t Keypad::read_key(void)
{
    uint16_t adcval = analogRead(_key_pin);

    if ((adcval >= _keyADCValues[0]) && (adcval <= _keyADCValues[1]))
    {
        _last_key = KEYPAD_KEY_RIGHT;
        return KEYPAD_KEY_RIGHT;
    }

    if ((adcval >= _keyADCValues[2]) && (adcval <= _keyADCValues[3]))
    {
        _last_key = KEYPAD_KEY_UP;
        return KEYPAD_KEY_UP;
    }

    if ((adcval >= _keyADCValues[4]) && (adcval <= _keyADCValues[5]))
    {
        _last_key = KEYPAD_KEY_DOWN;
        return KEYPAD_KEY_DOWN;
    }

    if ((adcval >= _keyADCValues[6]) && (adcval <= _keyADCValues[7]))
    {
        _last_key = KEYPAD_KEY_LEFT;
        return KEYPAD_KEY_LEFT;
    }

    if ((adcval >= _keyADCValues[8]) && (adcval <= _keyADCValues[9]))
    {
        _last_key = KEYPAD_KEY_SELECT;
        return KEYPAD_KEY_SELECT;
    }

    if ((adcval >= _keyADCValues[10]) && (adcval <= _keyADCValues[11]))
    {
        _last_key = KEYPAD_KEY_NONE;
        return KEYPAD_KEY_NONE;
    }

    // return key 'none' if nothing else matches
    return KEYPAD_KEY_NONE;
}

// Return the last key that was read
uint8_t Keypad::get_last_key(void)
{
    return _last_key;
}


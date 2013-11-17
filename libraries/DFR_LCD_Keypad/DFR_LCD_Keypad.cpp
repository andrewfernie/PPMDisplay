/*
	Arduino LCD + Keypad Library
	by Andy Gock
	http://micro.gock.net
*/
#include "DFR_LCD_Keypad.h"

DFR_LCD_Keypad::DFR_LCD_Keypad()
{
	DFR_LCD_Keypad::DFR_LCD_Keypad(DFR_LCD_KEYPAD_KEY_PIN);
}

DFR_LCD_Keypad::DFR_LCD_Keypad(const uint8_t key_pin)
{
	// Make sure pin 10 is input, if it set to output and put high, it will cause the pin
	// to drive the backlight directly, which will exceed the maximum output load of the
	// MCU IO pins
	this->backlight_on();
	
	_key_pin = key_pin;
	_last_key = DFR_LCD_KEYPAD_KEY_NONE;
}

DFR_LCD_Keypad::DFR_LCD_Keypad(const uint8_t key_pin, LiquidCrystal *lcd)
{
	_lcd = lcd;
	DFR_LCD_Keypad::DFR_LCD_Keypad(A0);
}

void DFR_LCD_Keypad::backlight_off(void)
{
	pinMode(DFR_LCD_KEYPAD_BACKLIGHT_PIN, OUTPUT);
	digitalWrite(DFR_LCD_KEYPAD_BACKLIGHT_PIN, 0);
}

void DFR_LCD_Keypad::backlight_on(void)
{
	pinMode(DFR_LCD_KEYPAD_BACKLIGHT_PIN, INPUT);
}

uint8_t DFR_LCD_Keypad::read_key(void)
{
	uint16_t adcval = analogRead(_key_pin);
	
	if ((adcval >= DFR_LCD_KEYPAD_KEY_RIGHT_ADC_LOW) && (adcval <= DFR_LCD_KEYPAD_KEY_RIGHT_ADC_HIGH)) {
		_last_key = DFR_LCD_KEYPAD_KEY_RIGHT;
		return DFR_LCD_KEYPAD_KEY_RIGHT;
	}

	if ((adcval >= DFR_LCD_KEYPAD_KEY_UP_ADC_LOW) && (adcval <= DFR_LCD_KEYPAD_KEY_UP_ADC_HIGH)) {
		_last_key = DFR_LCD_KEYPAD_KEY_UP;
		return DFR_LCD_KEYPAD_KEY_UP;
	}

	if ((adcval >= DFR_LCD_KEYPAD_KEY_DOWN_ADC_LOW) && (adcval <= DFR_LCD_KEYPAD_KEY_DOWN_ADC_HIGH)) {
		_last_key = DFR_LCD_KEYPAD_KEY_DOWN;
		return DFR_LCD_KEYPAD_KEY_DOWN;
	}

	if ((adcval >= DFR_LCD_KEYPAD_KEY_LEFT_ADC_LOW) && (adcval <= DFR_LCD_KEYPAD_KEY_LEFT_ADC_HIGH)) {
		_last_key = DFR_LCD_KEYPAD_KEY_LEFT;
		return DFR_LCD_KEYPAD_KEY_LEFT;
	}

	if ((adcval >= DFR_LCD_KEYPAD_KEY_SELECT_ADC_LOW) && (adcval <= DFR_LCD_KEYPAD_KEY_SELECT_ADC_HIGH)) {
		_last_key = DFR_LCD_KEYPAD_KEY_SELECT;
		return DFR_LCD_KEYPAD_KEY_SELECT;
	}
	
	if ((adcval >= DFR_LCD_KEYPAD_KEY_NONE_ADC_LOW) && (adcval <= DFR_LCD_KEYPAD_KEY_NONE_ADC_HIGH)) {
		_last_key = DFR_LCD_KEYPAD_KEY_NONE;
		return DFR_LCD_KEYPAD_KEY_NONE;
	}	
	
	// return key 'none' if nothing else matches
	return DFR_LCD_KEYPAD_KEY_NONE;
}

// Return the last key that was read
uint8_t DFR_LCD_Keypad::get_last_key(void)
{
	return _last_key;
}

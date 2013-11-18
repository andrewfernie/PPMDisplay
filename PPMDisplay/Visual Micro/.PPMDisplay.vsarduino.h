#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Nano w/ ATmega328
#define __AVR_ATmega328P__
#define 
#define ARDUINO 101
#define ARDUINO_MAIN
#define __AVR__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
//
void read_ppm();
void reset_display();

#include "C:\arduino\hardware\arduino\variants\eightanaloginputs\pins_arduino.h" 
#include "C:\arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\andrew\Documents\Arduino\LocalPPMDisplay\PPMDisplay\PPMDisplay.ino"
#endif

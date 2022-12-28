/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 28 September 2018 by Georgi Angelov  
*/

#ifndef Arduino_h
#define Arduino_h

#include <interface.h>
#include <variant.h>
#include "minmax.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "pgmspace.h"
#include "binary.h"
#include "constants.h"

  uint32_t millis(void);
  uint32_t micros(void);
  void delay(uint32_t);
  void delayMicroseconds(uint32_t us);

  void pinMode(uint8_t, uint8_t);
  void digitalWrite(uint8_t, uint8_t);
  int digitalRead(uint8_t);

  void analogInit(uint8_t);
  int analogRead(uint8_t);
  void analogWrite(uint8_t, int);

  void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
  uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

  unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
  unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);
  void yield(void);
  void interrupts(void);
  void noInterrupts(void);
  void attachInterrupt(uint8_t, void (*)(void), int mode);
  void detachInterrupt(uint8_t);

#ifdef __cplusplus
} // extern "C"

////////////////////////////////////////////////////////////////////////////////////////////////

#include "WCharacter.h"
#include "WString.h"

/* WMath */
long random(long);
long random(long howsmall, long howbig);
void randomSeed(unsigned long seed);
long map(long x, long in_min, long in_max, long out_min, long out_max);
uint32_t makeWord(uint32_t w);
uint32_t makeWord(unsigned char h, unsigned char l);

void noTone(uint8_t _pin);
void tone(uint8_t _pin, uint32_t frequency, unsigned long duration = 0);

#endif //__cplusplus

#endif //Arduino_h

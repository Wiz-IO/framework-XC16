///////////////////////////////////////////////////////////////////////////////
//
//      WizIO 2022 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "variant.h"

void enableAnalogPin(int an)
{
    switch (an)
    {
    case 9:
        _ANSB9 = 1;
        break;
    case 10:
        _ANSB10 = 1;
        break;
    case 11:
        _ANSB11 = 1;
        break;        
    }
}

pin_t pins[PIN_MAX] = {
    {&TRISE, &LATE, &PORTE, 1 << 0},
    {&TRISE, &LATE, &PORTE, 1 << 1},
    {&TRISE, &LATE, &PORTE, 1 << 2},
    {&TRISE, &LATE, &PORTE, 1 << 3},
};

uint32_t get_timer(void)
{
    uint16_t lo = TMR2;
    uint16_t hi = TMR3HLD;
    return (((uint32_t)hi << 16) | lo) >> 1; // us
}

void TMR2_Init(void)
{
    TMR3 = 0;
    PR3 = -1;
    TMR2 = 0;
    PR2 = -1;
    T2CON = 0x8018; // 2MHz
}

void CLOCK_Init(void) // 32MHz --> 16MHz
{
    CLKDIV = 0x3000;

    // Initiate Clock Switch to Internal FRC with PLL
    __builtin_write_OSCCONH((uint8_t)(0x01));
    __builtin_write_OSCCONL((uint8_t)(0x01));

    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN != 0)
        ;
    while (OSCCONbits.LOCK != 1)
        ;
}

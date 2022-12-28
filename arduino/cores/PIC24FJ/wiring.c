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

#include "interface.h"
#include <libpic30.h>

extern uint32_t get_timer(void);

uint32_t micros(void) { return get_timer(); }

uint32_t millis(void) { return get_timer() / 1000; }

uint32_t seconds(void) { return millis() / 1000; }

void delay(uint32_t ms)
{
    while (ms--)
    {
        __delay_ms(1);
    }
}

void delayMicroseconds(uint32_t us)
{
    while (us >= 32)
    {
        __delay_us(32);
        us -= 32;
    }
    while (us--)
    {
        __delay_us(1);
    }
}

///////////////////////////////////////////////////////////////////////////////

void analogWrite(uint8_t pin, int val) {}

int analogRead(uint8_t channel)
{
    AD1CHS = channel & 31;

    extern void enableAnalogPin(int an);
    enableAnalogPin(channel); // ENABLE PIN

    if (0 == AD1CON1bits.ADON)
    {
        AD1CON1 = 0x0000;
        AD1CON2 = 0x0000;
        AD1CON3 = 0x0002;
        AD1CSSL = 0x0000;
        AD1CON1bits.ASAM = 1;
        AD1CON1bits.SSRC = 0b111;
        AD1CON3bits.ADCS = 0b11111111; // 256 * TCY
        AD1CON3bits.SAMC = 0b11111;    // 31 TAD
        AD1CON1bits.ADON = 1;
    }

    delay(1); // ?!

    _SAMP = 1;
    while (!_DONE)
        continue;
    return ADC1BUF0;
}

///////////////////////////////////////////////////////////////////////////////
#define mRtccIsOn() (RCFGCALbits.RTCEN)
#define mRtccIsWrEn() (RCFGCALbits.RTCWREN)
#define mRtccOff() (RCFGCALbits.RTCEN = 0)
#define mRtccWrOff() (RCFGCALbits.RTCWREN = 0)

static void rtccWrOn(void)
{
    uint8_t CPU_IPL;
    unsigned int data;

    /* Disable Global Interrupt */
    SET_AND_SAVE_CPU_IPL(CPU_IPL, 7);

    asm volatile("MOV #0x55,%0"
                 : "=r"(data));
    asm volatile("MOV %0,NVMKEY"
                 :
                 : "r"(data)); // Write the 0x55 key
    asm volatile("MOV #0xAA,%0"
                 : "=r"(data));
    asm volatile("MOV %0,NVMKEY"
                 :
                 : "r"(data)); // Write the 0xAA key

    RCFGCALbits.RTCWREN = 1;

    /* Enable Global Interrupt */
    RESTORE_CPU_IPL(CPU_IPL);
}

static void rtccInitClock(void)
{
    __builtin_write_OSCCONL(2);
    RCFGCAL = 0x0;
    if (mRtccIsOn())
    {
        if (!mRtccIsWrEn())
            rtccWrOn();
        mRtccOff();
    }
    mRtccWrOff();
}

void rtccInit(void)
{
    rtccInitClock();
    rtccWrOn();
}

///////////////////////////////////////////////////////////////////////////////

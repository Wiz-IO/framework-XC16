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

#ifndef INTERFACE_H_
#define INTERFACE_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>

#include <xc.h>

    extern int isalnum(int);
    extern int isalpha(int);
    extern int iscntrl(int);
    extern int isdigit(int);
    extern int isgraph(int);
    extern int islower(int);
    extern int isprint(int);
    extern int ispunct(int);
    extern int isspace(int);
    extern int isupper(int);
    extern int isxdigit(int);
    extern int tolower(int);
    extern int toupper(int);

    extern int isascii(int);
    extern int isblank(int);
    extern int toascii(int);

    extern int isnan(double);
    extern int isinf(double);

    void digitalToggle(uint8_t pin);
#define digitalPinToPort(p)
#define digitalPinToBitMask(p)
#define digitalPinToClkid(p)
#define digitalPinSPIAvailiable(p)
#define digitalPinToSPIDevice(p)
#define digitalPinToSPIClockId(p)

#define PPS_UNLOCK() __builtin_write_OSCCONL(OSCCON & 0xBF)
#define PPS_LOCK() __builtin_write_OSCCONL(OSCCON | 0x40)

    static inline void interrupts(void)
    {
        __asm__ volatile("disi #0x3FFF");
    }
    static inline void noInterrupts(void)
    {
        __asm__ volatile("disi #0x0000");
    }

    uint32_t seconds(void);
#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)                 /* ? */
#define clockCyclesToMicroseconds(a) (((a)*1000L) / (F_CPU / 1000L))   /* ? */
#define microsecondsToClockCycles(a) ((a)*clockCyclesPerMicrosecond()) /* ? */

#ifdef __cplusplus
}

#endif
#endif /* INTERFACE_H_ */
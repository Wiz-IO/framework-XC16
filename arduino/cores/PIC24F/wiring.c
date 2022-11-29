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
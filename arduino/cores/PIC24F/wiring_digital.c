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

#include <Arduino.h>

void pinMode(uint8_t pin, uint8_t mode)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if (mode == OUTPUT)
        {
            *p->tris &= ~p->mask;
        }
        else if (mode == OUTPUT_HI)
        {
            *p->tris &= ~p->mask;
            digitalWrite(pin, 1);
        }
        else if (mode == OUTPUT_LO)
        {
            *p->tris &= ~p->mask;
            digitalWrite(pin, 0);
        }
        else
        {
            *p->tris |= p->mask;
        }
    }
}

int digitalRead(uint8_t pin)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        return (bool)(*p->port & p->mask);
    }
    return 0;
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if (val)
        {
            *p->lat |= p->mask;
        }
        else
        {
            *p->lat &= ~p->mask;
        }
    }
}

void digitalToggle(uint8_t pin)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if ((bool)(*p->lat & p->mask))
            digitalWrite(pin, 0);
        else
            digitalWrite(pin, 1);
    }
}
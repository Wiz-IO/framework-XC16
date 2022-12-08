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

extern "C" void TMR2_Init(void);
extern "C" void CLOCK_Init(void);

void initVariant(void)
{
    CLOCK_Init(); // FCY = 16M
    TMR2_Init();  // F = 2M
    ANSB = 0x0000;
    ANSC = 0x0000;
    ANSD = 0x0000;
    ANSF = 0x0000;
    ANSG = 0x0000;
#ifdef __PIC24FJ256GB210__
    ANSA = 0x0000;
    ANSE = 0x0000;
#endif
}

Uart Serial(0);
Uart Serial1(1);
Uart Serial2(2);
Uart Serial3(3);

extern "C" void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) { Serial.IrqHandler(); }
extern "C" void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) { Serial1.IrqHandler(); }
extern "C" void __attribute__((interrupt, no_auto_psv)) _U3RXInterrupt(void) { Serial2.IrqHandler(); }
extern "C" void __attribute__((interrupt, no_auto_psv)) _U4RXInterrupt(void) { Serial3.IrqHandler(); }

uart_inst_t u_inst[UART_MAX] = {
    {(uart_reg_t *)&U1MODE, _U1RXInterrupt},
    {(uart_reg_t *)&U2MODE, _U2RXInterrupt},
    {(uart_reg_t *)&U3MODE, _U3RXInterrupt},
    {(uart_reg_t *)&U4MODE, _U4RXInterrupt},
};
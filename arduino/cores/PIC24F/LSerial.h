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

#ifndef _LUART_H_
#define _LUART_H_
#ifdef __cplusplus

#include "interface.h"
#include "HardwareSerial.h"
#include <RingBuffer.h>

typedef struct uart_reg_s
{
    uint16_t MODE;
    uint16_t STA;
    uint16_t TX;
    uint16_t RX;
    uint16_t BRG;
} uart_reg_t;

typedef struct uart_inst_s
{
    uart_reg_t *reg;
    void (*isr)(void);
} uart_inst_t;

extern uart_inst_t u_inst[UART_MAX];

class Uart : public HardwareSerial
{
private:
    uart_inst_t *uart;

    unsigned int id;

    void _pinInit()
    {
        PPS_UNLOCK();
        switch (id)
        {
        case 0:
            RPINR18bits.U1RXR = 18; // RX FUNC = RPIN
            RPOR14bits.RP28R = 3;   // TX RPIN = MODE
            break;
            // TODO OTHER
        }
        PPS_LOCK();
    }

    void _setRxIF(unsigned v)
    {
        switch (id)
        {
        case 0:
            _U1RXIF = v;
            return;
        case 1:
            _U2RXIF = v;
            return;
        case 2:
            _U3RXIF = v;
            return;
        case 3:
            _U4RXIF = v;
            return;
        }
    }

    void _setRxIE(unsigned v)
    {
        switch (id)
        {
        case 0:
            _U1RXIE = v;
            return;
        case 1:
            _U2RXIE = v;
            return;
        case 2:
            _U3RXIE = v;
            return;
        case 3:
            _U4RXIE = v;
            return;
        }
    }

    void _setRxIP(uint8_t v)
    {
        switch (id)
        {
        case 0:
            _U1RXIP = v;
            return;
        case 1:
            _U2RXIP = v;
            return;
        case 2:
            _U3RXIP = v;
            return;
        case 3:
            _U4RXIP = v;
            return;
        }
    }

    RingBuffer rxBuffer;

public:
    void setRPins(uint8_t RPIN_TX, uint8_t RPIN_RX)
    {
        PPS_UNLOCK();
        uint8_t *p = (uint8_t *)&RPOR0;
        switch (id)
        {
        case 0:
            RPINR18bits.U1RXR = RPIN_RX;
            p[RPIN_TX] = 3; // = mode
            break;
        case 1:
            RPINR19bits.U2RXR = RPIN_RX;
            p[RPIN_TX] = 5; 
            break;
        case 2:
            RPINR17bits.U3RXR = RPIN_RX;
            p[RPIN_TX] = 28; 
            break;
        case 3:
            RPINR27bits.U4RXR = RPIN_RX;
            p[RPIN_TX] = 30;
            break;
        }
        PPS_LOCK();
    }

    Uart(unsigned int _id, unsigned int prio = 2)
    {
        id = _id;
        uart = &u_inst[id];
        _setRxIP(prio);
        _pinInit();
    }

    ~Uart() { end(); }

    void begin(unsigned long baudrate) // 8n1
    {
        rxBuffer.clear();
        uart->reg->BRG = (((FCY / baudrate) / 4) - 1);
        uart->reg->MODE = 0xC888;
        uart->reg->STA = 0x0510;
        _setRxIF(0);
        _setRxIE(1);
    }

    void begin(unsigned long baudrate, unsigned long config) {} // TODO

    void end() { uart->reg->STA = 0; }

    int available() { return rxBuffer.available(); }

    int peek() { return rxBuffer.peek(); }

    void flush()
    {
#define SPEN 0x8000u
#define TXEN 0x0400u
#define TRMT 0x0100u
        if ((uart->reg->MODE & SPEN) && (uart->reg->STA & TXEN))
        {
            while ((uart->reg->STA & TRMT) == 0)
                ;
        }
        rxBuffer.clear();
    }

    int read()
    {
        int c = rxBuffer.read_char();
        return c;
    }

    size_t write(uint8_t c)
    {
        while (uart->reg->STA & 1 << 9)
            ;
        uart->reg->TX = c;
        return 1;
    }

    using Print::write;
    operator bool() { return 1; }

    void IrqHandler()
    {
        while (uart->reg->STA & 1) // URXDA
            rxBuffer.store_char(uart->reg->RX);
        _setRxIF(id);
    }
};

extern Uart Serial;
extern Uart Serial1;
extern Uart Serial2;
extern Uart Serial3;

#endif
#endif // _LUART_H_
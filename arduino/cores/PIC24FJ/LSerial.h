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

#include <interface.h>
#include <HardwareSerial.h>
#include <RingBuffer.h>

#define SERIAL_START_BIT_1 (0)
#define SERIAL_START_BIT_2 (1)

#define SERIAL_8_NONE (0)
#define SERIAL_8_EVEN (2)
#define SERIAL_8_ODD (4)
#define SERIAL_9_NONE (6)

#define SERIAL_8N1 (SERIAL_8_NONE | SERIAL_START_BIT_1)
#define SERIAL_8N2 (SERIAL_8_NONE | SERIAL_START_BIT_2)

#define SERIAL_8E1 (SERIAL_8_EVEN | SERIAL_START_BIT_1)
#define SERIAL_8E2 (SERIAL_8_EVEN | SERIAL_START_BIT_2)

#define SERIAL_8O1 (SERIAL_8_ODD | SERIAL_START_BIT_1)
#define SERIAL_8O2 (SERIAL_8_ODD | SERIAL_START_BIT_2)

#define SERIAL_9N1 (SERIAL_9_NONE | SERIAL_START_BIT_1)
#define SERIAL_9N2 (SERIAL_9_NONE | SERIAL_START_BIT_2)

typedef struct
{
    volatile union
    {
        uint16_t MODE;
        UxMODEBITS mode;
    };
    volatile union
    {
        uint16_t STA;
        UxSTABITS sta;
    };
    volatile uint16_t TX;
    volatile uint16_t RX;
    volatile uint16_t BRG;
} uart_reg_t;

class Uart : public HardwareSerial
{
private:
    unsigned int id;

    uart_reg_t *uart;

    void _initDefaultPins()
    {
        PPS_UNLOCK();
        switch (id)
        {
        case 0:
            RPINR18bits.U1RXR = 18; // RX FUNC = RPIN
            RPOR14bits.RP28R = 3;   // TX RPIN = MODE
            break;
        }
        PPS_LOCK();
    }

    void _rxIF(unsigned v)
    {
        switch (id)
        {
        case 1:
            _U2RXIF = v;
            return;
        case 2:
            _U3RXIF = v;
            return;
        case 3:
            _U4RXIF = v;
            return;
        default:
            _U1RXIF = v;
        }
    }

    void _rxIE(unsigned v)
    {
        switch (id)
        {
        case 1:
            _U2RXIE = v;
            return;
        case 2:
            _U3RXIE = v;
            return;
        case 3:
            _U4RXIE = v;
            return;
        default:
            _U1RXIE = v;
        }
    }

    void _rxIP(uint8_t v)
    {
        switch (id)
        {
        case 1:
            _U2RXIP = v;
            return;
        case 2:
            _U3RXIP = v;
            return;
        case 3:
            _U4RXIP = v;
            return;
        default:
            _U1RXIP = v;
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

    Uart(unsigned int _id)
    {
        id = _id;
        switch (id)
        {
        case 1:
            uart = (uart_reg_t *)&U2MODE;
            break;
        case 2:
            uart = (uart_reg_t *)&U3MODE;
            break;
        case 3:
            uart = (uart_reg_t *)&U4MODE;
            break;
        default:
            uart = (uart_reg_t *)&U1MODE;
            break;
        }
        _rxIP(2);
        _initDefaultPins();
    }

    ~Uart() { end(); }

    void end()
    {
        uart->MODE = 0;
        uart->STA = 0;
        _rxIE(0);
        _rxIF(0);
        rxBuffer.clear();
    }

    void begin(unsigned long baudrate, unsigned long config)
    {
        end();
        uart->BRG = (((FCY / baudrate) / 4) - 1);
        uart->mode.BRGH = 1;
        uart->mode.STSEL = config & 1;
        uart->mode.PDSEL = (config >> 1) & 3;
        uart->mode.UARTEN = 1;
        uart->sta.UTXEN = 1;
        _rxIE(1);
    }

    void begin(unsigned long baudrate) { begin(baudrate, SERIAL_8N1); }

    void begin() { begin(115200, SERIAL_8N1); }

    int available() { return rxBuffer.available(); }

    int peek() { return rxBuffer.peek(); }

    void flush()
    {
#define SPEN 0x8000u
#define TXEN 0x0400u
#define TRMT 0x0100u
        if ((uart->MODE & SPEN) && (uart->STA & TXEN))
        {
            while ((uart->STA & TRMT) == 0)
                continue;
        }
        rxBuffer.clear();
    }

    int read() { return rxBuffer.read_char(); }

    size_t write(uint8_t c)
    {
        while (uart->STA & 1 << 9) // UTXBF
            continue;
        uart->TX = c;
        return 1;
    }

    using Print::write;

    operator bool() { return uart->mode.UARTEN; }

    void IrqHandler()
    {
        while (uart->STA & 1) // URXDA
            rxBuffer.store_char(uart->RX);
        _rxIF(0);
    }
};

extern Uart Serial;
extern Uart Serial1;
extern Uart Serial2;
extern Uart Serial3;

#endif
#endif // _LUART_H_
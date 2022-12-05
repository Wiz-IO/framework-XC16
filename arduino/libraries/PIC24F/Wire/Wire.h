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

#ifndef __WIRE_H__
#define __WIRE_H__
#ifdef __cplusplus

#include "interface.h"
#include "Stream.h"
#include "variant.h"
#include "RingBuffer.h"

#define PRINT Serial.printf

typedef struct I2C_s
{
    volatile uint16_t RCV;
    volatile uint16_t TRN;
    volatile uint16_t BRG;
    volatile uint16_t CON;
    volatile uint16_t STAT;
} I2C_t;

class TwoWire : public Stream
{
#define I2C_READ 1
#define I2C_WRITE 0
private:
    int _id;
    volatile I2C_t *i2c;
    uint32_t _speed, _timeout;
    bool transmissionBegun;
    uint8_t _slave_address;
    RingBuffer rx, tx;

    inline void _stop(void)
    {
        i2c->CON |= 1 << 2; /* PEN[2] */
        while (i2c->CON & 4)
            ;
    }

    inline void _restart(void)
    {
        i2c->CON |= 1 << 1; /*RSEN[1] */
        while (i2c->CON & 2)
            ;
    }

    inline void _start(void)
    {
        i2c->CON |= 1 << 0; /* SEN[0] */
        while (i2c->CON & 1)
            ;
    }

    void _ack(bool ack)
    {
        if (ack)
        {
            i2c->CON |= 1 << 5; // ACKDT[5]
        }
        else
        {
            i2c->CON &= ~(1 << 5); // ACKDT[5]
        }
        i2c->CON |= 1 << 4;       // ACKEN[4]
        while (i2c->CON & 1 << 4) // ACKEN[4]
            ;
    }

    int _idle(void)
    {
        uint32_t T = _timeout;
        while (T--)
        {
            if (!(i2c->CON & 0b11111)) // SEN[0] RSEN[1] PEN[2] RCEN[3] ACKEN[4]]
                break;
            if (!(i2c->STAT & 1 << 14)) // TRSTAT[14]
                break;
        }
        if (0 == T) // Timeout
            return -1;
        return (i2c->STAT & 1 << 15) ? -2 : 0; // ACKSTAT[15]
    }

    int _send(unsigned char data)
    {
        if (_idle())
            return -1;
        i2c->TRN = data;
        if (i2c->STAT & 1 << 7) // IWCOL[7]
        {
            i2c->STAT &= ~(1 << 7); // IWCOL[7]
            return -2;
        }
        uint32_t T = _timeout;
        while (i2c->STAT & 1 << 0) // TBF[0]
            if (0 == T--)
                return -3;
        while (i2c->STAT & 1 << 14) // TRSTAT[14] wait for the ACK
            if (0 == T--)
                return -4;
        return (i2c->STAT & 1 << 15) ? -5 : 0; // ACKSTAT[15] test for ACK condition received
    }

    int _recv()
    {
        if (_idle())
            return -1;
        i2c->CON |= 1 << 3;       // RCEN[3]
        while (i2c->CON & 1 << 3) // RCEN[3]
            ;
        return i2c->RCV;
    }

    int i2c_start(bool operation)
    {
        int res = -1;
        _start();
        if ((res = _send(_slave_address | operation)))
        {
            PRINT("[I2C] Write( ADDR ) = %d 0x%X\n", res, (int)i2c->STAT);
            return res;
        }
        if ((res = _idle()))
        {
            PRINT("[I2C] Idle( ADDR ) = %d\n", res);
        }
        return res;
    }

public:
    TwoWire(int id)
    {
        _id = id;
        switch (_id)
        {
        case 0:
            i2c = (I2C_t *)&I2C1RCV;
            break;
        case 1:
            i2c = (I2C_t *)&I2C2RCV;
            break;
        case 2:
            i2c = (I2C_t *)&I2C3RCV;
            break;
        }
        _speed = 100000;
        _timeout = 0x8000;
        _slave_address = 0x3C << 1;
        transmissionBegun = false;
    }

    ~TwoWire() { end(); }

    void end() { i2c->CON = 0; }

    void begin(uint8_t address_8b)
    {
        _slave_address = address_8b;
        i2c->BRG = 157;
        i2c->CON = 0x8000;
        flush();
        _stop(); // set bus to idle
    }

    void begin(void) { begin(_slave_address); }

    void setTimeOut(uint32_t timeout) { _timeout = timeout; }

    void setClock(uint32_t speed_Hz)
    {
        if (_speed != speed_Hz)
        {
            _speed = speed_Hz;
            i2c->BRG = (FCY / _speed) - (FCY / 10000000U) - 1;
            PRINT("[I2C] BRG = %u\n", (int)i2c->BRG);
        }
    }

    void beginTransmission(uint8_t address_8b)
    {
        _slave_address = address_8b;
        tx.clear();
        transmissionBegun = true;
    }

    uint8_t endTransmission(bool);
    uint8_t endTransmission() { return endTransmission(true); }

    uint8_t requestFrom(uint8_t address, size_t size, bool stopBit);
    uint8_t requestFrom(uint8_t address, size_t size) { return requestFrom(address, size, true); }

    size_t write(uint8_t ucData)
    {
        if (!transmissionBegun || tx.isFull())
            return 0;
        tx.store_char(ucData);
        return 1;
    }

    size_t write(const uint8_t *data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (!write(data[i]))
                return i;
        }
        return size;
    }

    virtual int available(void) { return rx.available(); }
    virtual int read(void) { return rx.read_char(); }
    virtual int peek(void) { return rx.peek(); }
    virtual void flush(void)
    {
        tx.clear();
        rx.clear();
        int r = i2c->RCV; // read to clear
        i2c->STAT = 0x00;
    }

    using Print::write;

    void onService(void){};
    void onReceive(void (*)(int)){};
    void onRequest(void (*)(void)){};
};

extern TwoWire Wire;
extern TwoWire Wire1;
extern TwoWire Wire2;

#endif // cpp
#endif // h
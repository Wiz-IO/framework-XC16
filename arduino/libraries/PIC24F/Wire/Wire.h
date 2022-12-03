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

//#define WIRE_PRINT Serial.printf

typedef struct I2C_s
{
    uint16_t RCV;
    uint16_t TRN;
    uint16_t BRG;
    uint16_t CON;
    uint16_t STAT;
    uint16_t ADD;
    uint16_t MSK;
} I2C_t;

class TwoWire : public Stream
{
private:
    int _id;
    I2C_t *i2c;
    uint32_t _speed, _timeout;
    bool transmissionBegun;
    uint8_t _slave_address;
    RingBuffer rx, tx;

    inline void RestartI2C(void)
    {
        i2c->CON |= 1 << 3; /*RSEN initiate restart on SDA and SCL pins */
    }
    inline void StopI2C(void)
    {
        i2c->CON |= 1 << 2; /* PEN, initiate Stop on SDA and SCL pins */
    }
    inline void StartI2C(void)
    {
        i2c->CON |= 1; /* SEN,  initiate Start on SDA and SCL pins */
    }

    int IdleI2C(void)
    {
        uint32_t T = _timeout;
        while (--T)
        {
            /* Wait until I2C Bus is Inactive */
            if (i2c->CON & 0b11111) // SEN RSEN PEN RCEN ACKEN
                return 0;
            if (i2c->STAT & 1 << 14) // TRSTAT
                return 0;
        }
        return -1;
    }

    int MasterWriteI2C(unsigned char data)
    {
        I2C1TRN = data;
        if (i2c->STAT & 1 << 7) /* IWCOL, If write collision occurs */
            return -1;
        else
            return 0;
    }

public:
    TwoWire(int id, uint32_t speed_Hz = 100000U)
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
        }
        _speed = speed_Hz;
        _timeout = 0x8000;
        transmissionBegun = false;
    }

    ~TwoWire() { end(); }

    void end() { i2c->CON = 0; }

    void begin(int SDA_NOT_USED, int SCL_NOT_USED, uint8_t address)
    {
        _slave_address = address;
        setClock(_speed);
        i2c->CON = 0x8000;
        i2c->STAT = 0x00;
    }

    void begin(void) { begin(0, 0, _slave_address); }

    void setTimeOut(uint32_t timeout) { _timeout = timeout; }

    void setClock(uint32_t speed_Hz)
    {
        if (_speed != speed_Hz)
        {
            _speed = speed_Hz;
            i2c->BRG = (FCY / speed_Hz) - (FCY / 10000000U) - 1;
        }
    }

    void beginTransmission(uint8_t address)
    {
        _slave_address = address;
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
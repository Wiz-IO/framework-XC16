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

#define WIRE_PRINT // Serial.printf

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
    uint32_t _speed;
    bool transmissionBegun;
    uint8_t _slave_address;
    RingBuffer rx, tx;

public:
    TwoWire(int id, uint32_t speed_Hz = 100000U)
    {
        _id = id;
        _speed = speed_Hz;
        transmissionBegun = false;
    }

    ~TwoWire() { end(); }

    void setTimeOut(uint32_t timeout_millis) {}

    void setClock(uint32_t speed_Hz)
    {
        I2C1BRG = 157;
    }

    void begin(int SDA_NOT_USED, int SCL_NOT_USED, uint8_t address)
    {
        _slave_address = address;

        I2C1BRG = 157;
        I2C1CON = 0x8000;
        I2C1STAT = 0x00;
        IFS1bits.MI2C1IF = 0;
        IEC1bits.MI2C1IE = 0;
    }

    void begin(void) { begin(0, 0, _slave_address); }

    void end() { I2C1CON = 0; }

    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool stopBit);
    uint8_t endTransmission(void);

    uint8_t requestFrom(uint8_t address, size_t size, bool stopBit);
    uint8_t requestFrom(uint8_t address, size_t size);

    size_t write(uint8_t data);
    size_t write(const uint8_t *data, size_t size);

    virtual int available(void) { return rx.available(); }
    virtual int read(void) { return rx.read_char(); }
    virtual int peek(void) { return rx.peek(); }
    virtual void flush(void) {}

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
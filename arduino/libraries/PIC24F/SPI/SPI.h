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

#ifndef __SPI_CLASS_H__
#define __SPI_CLASS_H__
#ifdef __cplusplus

#include <Arduino.h>

#define PRINTF Serial.printf

typedef struct SPI_s
{
    uint16_t STAT;
    uint16_t CON1;
    uint16_t CON2;
    uint16_t not_used;
    uint16_t BUFF;
} SPI_t;

typedef enum
{
    SPI_MODE0 = 0, // CPOL : 0  | CPHA : 0
    SPI_MODE1,     // CPOL : 0  | CPHA : 1
    SPI_MODE2,     // CPOL : 1  | CPHA : 0
    SPI_MODE3      // CPOL : 1  | CPHA : 1
} SPIDataMode;

class SPISettings
{
public:
    SPISettings(uint32_t clockFrequency, uint8_t bitOrder, uint8_t dataMode)
    {
        clock = clockFrequency; // Hz
        order = bitOrder;
        mode = dataMode;
    }

    SPISettings()
    {
        clock = 1000000;  // Hz
        order = MSBFIRST; // 1
        mode = SPI_MODE0;
    }

private:
    uint32_t clock;
    uint8_t order;
    uint8_t mode;
    friend class SPIClass;
};

class SPIClass
{
private:
    int _id;
    SPI_t *spi;

    uint32_t _brg_hz;
    int _clk_polarity;
    int _clk_format;
    int _bit_order;
    int _data_bits;
    int _mode;

public:
    ~SPIClass() { end(); }
    SPIClass(int id)
    {
        _id = id;
        switch (_id)
        {
        case 0:
            spi = (SPI_t *)&SPI1STAT;
            break;
        case 1:
            spi = (SPI_t *)&SPI2STAT;
            break;
        case 2:
            spi = (SPI_t *)&SPI2STAT;
            break;
        }
        _bit_order = MSBFIRST;
    }

    void setRPins(int clock, int miso, int mosi, int ss)
    {
        PPS_UNLOCK();
        uint8_t *p = (uint8_t *)&RPOR0;
        switch (_id)
        {
        case 0:
            if (clock > -1)
                p[clock] = 8;
            if (mosi > -1)
                p[mosi] = 7;
            if (ss > -1)
                p[ss] = 9;
            if (miso > -1)
                RPINR20bits.SDI1R = miso;
            break;
        case 1: // TODO
            break;
        case 2: // TODO
            break;
        }
        PPS_LOCK();
    }

    void begin(int sck = -1, int miso = -1, int mosi = -1, int ss = -1)
    {
        setRPins(sck, miso, mosi, ss);
        spi->CON1 = 0x32;
        spi->CON2 = 0;
        spi->STAT = 0x8000;
    }

    void end() { spi->STAT = 0; }

    void setFrequency(uint32_t Hz)
    {
        if (_brg_hz != Hz)
        {
            _brg_hz = Hz;
            uint16_t timeout = (uint32_t)FCY / Hz;
            if (timeout == 0)
            {
                spi->CON1 |= 0b11111;
                return;
            }

            uint16_t primary = 0x0003;
            while (timeout != 0)
            {
                if (timeout > 8)
                {
                    primary--;
                    if ((timeout % 4) != 0)
                        timeout += 4;
                    timeout /= 4;
                }
                else
                {
                    break;
                }
            }
            timeout--;

            spi->CON1 |= ((~(timeout << 2)) & 0x1C) | (primary & 3);
        }
    }

    void setDataMode(uint8_t mode)
    {
        if (_mode != mode)
        {
            _mode = mode;
            // TODO
        }
    }

    void setBitOrder(uint8_t order)
    {
        if (_bit_order != order)
        {
            _bit_order = order;
            // TODO
        }
    }

    uint8_t transfer(uint8_t tx)
    {
        spi->BUFF = tx;
        while (0 == (spi->STAT & 1))
            ;
        return spi->BUFF;
    }

    uint16_t transfer(uint16_t data)
    {
        union
        {
            uint16_t val;
            struct
            {
                uint8_t lsb;
                uint8_t msb;
            };
        } t;
        t.val = data;
        if (_bit_order == LSBFIRST)
        {
            t.lsb = transfer(t.lsb);
            t.msb = transfer(t.msb);
        }
        else
        {
            t.msb = transfer(t.msb);
            t.lsb = transfer(t.lsb);
        }
        return t.val;
    }

    int transfer(uint8_t *buf, size_t count)
    {
        for (size_t i = 0; i < count; i++)
            transfer(*buf++);
        return count;
    }

    void write(uint8_t *buf, size_t count) { transfer(buf, count); }

    void write(uint16_t *buf, size_t count) {}

    void beginTransaction(SPISettings settings)
    {
        int stat = spi->STAT;
        spi->STAT = 0;
        setFrequency(settings.clock);
        setDataMode(settings.mode);
        setBitOrder(settings.order);
        spi->STAT = stat;
    }

    // not used
    void endTransaction(void) {}
    uint32_t getClockDivider() { return 0; }
    void setClockDivider(uint8_t div) {}
    void usingInterrupt(int interruptNumber) {}
    void notUsingInterrupt(int interruptNumber) {}
    void attachInterrupt(){};
    void detachInterrupt(){};
};

#endif // __cplusplus
#endif //__SPI_CLASS_H__
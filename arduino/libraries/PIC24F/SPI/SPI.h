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

typedef struct SPI_s
{
    uint16_t STAT;
    uint16_t CON1;
    uint16_t CON2;
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

    int8_t _ss;

    void _clrIF()
    {
        switch (_id)
        {
        case 0:
            _SPI1IF = 0;
            break;
        case 1:
            _SPI2IF = 0;
            break;
        case 2:
            _SPI3IF = 0;
            break;
        }
    }

    unsigned _getIF()
    {
        switch (_id)
        {
        case 0:
            return _SPI1IF;
        case 1:
            return _SPI2IF;
        case 2:
            return _SPI3IF;
        }
    }

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
        _ss = -1;
    }

    void begin(int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1, int8_t ss = -1)
    {
        // TODO
        _ss = ss;
        if (ss > -1)
            pinMode(ss, OUTPUT);
        spi->STAT |= 0x8000;
    }

    void end()
    {
        // if (_ss > -1) pinMode(_ss, INPUT);
        spi->STAT = 0;
    }

    void setFrequency(uint32_t Hz)
    {
        if (_brg_hz != Hz)
        {
            _brg_hz = Hz;
            uint32_t timeout = FCY / Hz;
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
            uint16_t secondary = (~(timeout << 2)) & 0x1C;
            // TODO
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
        // if (_ss > -1) digitalWrite(_ss, 0);
        _clrIF();
        spi->BUFF = tx;
        while (_getIF() == 0)
            ;
        uint8_t rx = spi->BUFF;
        // if (_ss > -1) digitalWrite(_ss, 1);
        return rx;
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
        if (_ss > -1)
            digitalWrite(_ss, 0);
        setFrequency(settings.clock);
        setDataMode(settings.mode);
        setBitOrder(settings.order);
        // TODO
    }

    void endTransaction(void)
    {
        if (_ss > -1)
            digitalWrite(_ss, 1);
    }

    // not used
    uint32_t getClockDivider() { return 0; }
    void setClockDivider(uint8_t div) {}
    void usingInterrupt(int interruptNumber) {}
    void notUsingInterrupt(int interruptNumber) {}
    void attachInterrupt(){};
    void detachInterrupt(){};
};

#endif // __cplusplus
#endif //__SPI_CLASS_H__
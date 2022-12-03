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
#include "Wire.h"

uint8_t TwoWire::requestFrom(uint8_t address, size_t size, bool stopBit) // TODO
{
    _slave_address = address;
    if (size == 0)
        return 0;
    if (!stopBit)
        return 0;
    rx.clear();

    // READ

    rx._iHead = size;
    return rx.available();
}

uint8_t TwoWire::endTransmission(bool stopBit) // TODO
{
    if (!stopBit)
        return 1;
    int res = 0;
    transmissionBegun = false;
    int size = tx.available();

    // WRITE

    StartI2C();
    if (IdleI2C() || (i2c->STAT & 1 << 15)) // ACKSTAT
        return 4;
    MasterWriteI2C(_slave_address | 0);
    if (IdleI2C() || (i2c->STAT & 1 << 15)) // ACKSTAT
        return 4;
    while (tx.available() > 0)
    {
        MasterWriteI2C(tx.read_char());
        if (IdleI2C() || (i2c->STAT & 1 << 15)) // ACKSTAT
            return 4;
    }
    StopI2C();
    return IdleI2C(); // 0:success
}

TwoWire Wire(0);
TwoWire Wire1(1);
TwoWire Wire2(2);
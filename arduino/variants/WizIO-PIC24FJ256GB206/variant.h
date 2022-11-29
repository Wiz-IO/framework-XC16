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

#ifndef __VARIANT_H__
#define __VARIANT_H__

#include <interface.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* C ware */

#define PIN_MAX 4
    typedef struct
    {
        volatile unsigned int *tris;
        volatile unsigned int *lat;
        volatile unsigned int *port;
        unsigned int mask;
    } pin_t;
    extern pin_t pins[PIN_MAX];

#define LED 3

#ifdef __cplusplus
}

/* CPP ware */

#define UART_MAX 4 
#include <LSerial.h>
extern Uart Serial;

#endif //__cplusplus
#endif /* __VARIANT_H__ */
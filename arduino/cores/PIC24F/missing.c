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

#include <stdint.h>
#include <ctype.h>
#include <xc.h>

// https://github.com/rofl0r/musl/tree/master/src/ctype

int isascii(int c) { return !(c & ~0x7f); }

int isblank(int c) { return (c == ' ' || c == '\t'); }

int isalpha(int c) { return ((unsigned)c | 32) - 'a' < 26; }

int isdigit(int c) { return (unsigned)c - '0' < 10; }

int isalnum(int c) { return isalpha(c) || isdigit(c); }

int iscntrl(int c) { return (unsigned)c < 0x20 || c == 0x7f; }

int toascii(int c) { return c & 0x7f; }

///////////////////////////////////////////////////////////////////////////////

int isnan(double f)
{
    if (f == f)
        return 0;
    else
        return -1;
}

int isinf(double f) // ???
{
    unsigned long l;
    l = *((long *)&f);
    if (l == 0x7F800000)
        return 1;
    if (l == 0xFF800000)
        return -1;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

__attribute__((noreturn, naked)) void __cxa_pure_virtual()
{
    //asm("reset");
    while (1)
        ;
}

///////////////////////////////////////////////////////////////////////////////

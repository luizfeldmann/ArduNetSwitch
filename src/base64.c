#include "base64.h"
#include <stdint.h>
#include <avr/pgmspace.h>

/* Algorithm copied (with mods) from Wikibooks (public domain) */

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] PROGMEM = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64_decode(const char* pEncInput, size_t uInputLen, char* pDecOutput, size_t* puDecOutLen)
{
    const char* end = pEncInput + uInputLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;

    while (pEncInput < end)
    {
        unsigned char c = pgm_read_byte(&d[*pEncInput++]);

        switch (c)
        {
        case WHITESPACE:
            continue;   // skip whitespace

        case INVALID:
            return 1;   // invalid input, return error

        case EQUALS:
            pEncInput = end;   // pad character, end of data
            continue;

        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration

            // If the buffer is full, split it into bytes
            if (iter == 4)
            {
                if ((len += 3) > *puDecOutLen)
                    return 1; // buffer overflow

                *(pDecOutput++) = (buf >> 16) & 255;
                *(pDecOutput++) = (buf >> 8) & 255;
                *(pDecOutput++) = buf & 255;
                buf = 0;
                iter = 0;
            }
        }
    }

    if (iter == 3)
    {
        if ((len += 2) > *puDecOutLen)
            return 1; // buffer overflow

        *(pDecOutput++) = (buf >> 10) & 255;
        *(pDecOutput++) = (buf >> 2) & 255;
    }
    else if (iter == 2)
    {
        if (++len > *puDecOutLen)
            return 1; // buffer overflow

        *(pDecOutput++) = (buf >> 4) & 255;
    }

    *puDecOutLen = len; // modify to reflect the actual output size
    return 0;
}
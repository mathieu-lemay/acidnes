#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

void _log_print(const char *tag, const char *fmt, va_list args);

void _log_print(const char *tag, const char *fmt, va_list args) {
    fprintf(stdout, "[%s]: ", tag);
    vfprintf(stdout, fmt, args);
}

void _log(const char *tag, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    _log_print(tag, fmt, args);
    va_end(args);
}

void _panic(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    _log_print("FATAL", fmt, args);
    va_end(args);

    exit(1);
}

#ifdef DEBUG
void _debug_log(const char *tag, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    _log_print(tag, fmt, args);
    va_end(args);
}
#else
inline void _debug_log(const char *tag, const char *fmt, ...) {}
#endif

uint8_t get_bit_at(uint8_t c, uint8_t pos) {
    if (pos >= 8)
        return 0xff;

    return ((uint8_t) (c >> pos) & 0x1u);
}

inline uint16_t u8_to_u16(uint8_t lo, uint16_t hi) {
    return lo | (uint16_t) (hi << 8u);
}

void hexdump(const void *data, unsigned int offset, unsigned int size) {
    /* dumps size bytes of *data to stdout. Looks like:
     * [00000000] 75 6E 6B 6E 6F 77 6E 20
     *                  30 FF 00 00 00 00 39 00 unknown 0.....9.
     * (in a single line of course)
     */

    const unsigned char *p;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[16 * 3 + 5] = {0};
    char charstr[16 * 1 + 5] = {0};
    const char line_format[] = "[%s]  %-49s %s\n";

    data = ((void *) ((size_t) data + offset));
    p = (const unsigned char *) data;

    for (n = 0; n < size; n++) {
        if (n % 16 == 0) {
            if (n > 0) {
                printf(line_format, addrstr, hexstr, charstr);
            }

            hexstr[0] = 0;
            charstr[0] = 0;

            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.8lx",
                     ((size_t) p - (size_t) data + (size_t) offset));
        } else if (n % 8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, " ", sizeof(hexstr) - strlen(hexstr) - 1);
            strncat(charstr, " ", sizeof(charstr) - strlen(charstr) - 1);
        }

        c = *p;
        if (isalnum(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr) - strlen(hexstr) - 1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr) - strlen(charstr) - 1);

        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        printf(line_format, addrstr, hexstr, charstr);
    }
}

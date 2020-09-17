#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

/* Uncomment for debug output */
//#define DEBUG 1

void _log(const char *tag, const char *fmt, ...);
void _panic(const char *fmt, ...);
void _debug_log(const char *tag, const char *fmt, ...);

uint8_t get_bit_at(uint8_t c, uint8_t pos);
uint16_t u8_to_u16(uint8_t lo, uint16_t hi);

/* Dumps size bytes of *data to stdout starting at "offset". Looks like:
 * [0000] 75 6E 6B 6E 6F 77 6E 20   30 FF 00 00 00 00 39 00 unknown 0.....9.
 *
 * Source: http://sws.dett.de/mini/hexdump-c/
 */
void hexdump(const void *data, unsigned int offset, unsigned int size);

#ifdef __cplusplus
}
#endif
#endif /*__COMMON_H__*/

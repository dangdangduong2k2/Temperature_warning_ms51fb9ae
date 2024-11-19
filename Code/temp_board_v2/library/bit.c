#ifndef _BIT_C_
#define _BIT_C_
#include "stdbool.h"
#include "bit.h"

bool     bit_test(uint16_t word, uint8_t n) { return ((word >> n) & 0x0001); }
uint16_t bit_set(uint16_t word, uint8_t n) { return word | (1 << n); }
uint16_t bit_clr(uint16_t word, uint8_t n) { return word & ~(1 << n); }
uint16_t bit_write(uint16_t word, uint8_t n, uint8_t value) { return value ? bit_set(word, n) : bit_clr(word, n); }

#ifndef _BIT_8_C_
#define _BIT_8_C_
bool    bit_test_8(uint8_t dat, uint8_t n) { return ((dat >> n) & 0x01); }
uint8_t bit_set_8(uint8_t dat, uint8_t n) { return dat | (1 << n); }
uint8_t bit_clr_8(uint8_t dat, uint8_t n) { return dat & ~(1 << n); }
uint8_t bit_write_8(uint8_t dat, uint8_t n, uint8_t value) { return value ? bit_set_8(dat, n) : bit_clr_8(dat, n); }
#endif

#endif

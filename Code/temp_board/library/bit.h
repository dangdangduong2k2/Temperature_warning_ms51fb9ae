#ifndef _BIT_H_
#define _BIT_H_

bool     bit_test(uint16_t word, uint8_t n);
uint16_t bit_set(uint16_t word, uint8_t n);
uint16_t bit_clr(uint16_t word, uint8_t n);
uint16_t bit_write(uint16_t word, uint8_t n, uint8_t value);
#ifndef _BIT_8_H_
#define _BIT_8_H_
bool    bit_test_8(uint8_t dat, uint8_t n);
uint8_t bit_set_8(uint8_t dat, uint8_t n);
uint8_t bit_clr_8(uint8_t dat, uint8_t n);
uint8_t bit_write_8(uint8_t dat, uint8_t n, uint8_t value);
#endif

#endif

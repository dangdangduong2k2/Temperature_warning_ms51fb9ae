#ifndef _I2C_H_
#define _I2C_H_
#include "stdbool.h"
#include "bit.c"
#include "Config_GPIO.h"

#ifndef _DELAY_US_H_
#define _DELAY_US_H_
void delay_us(uint16_t t);
#endif

typedef struct I2C_Str_1 {
	uint8_t SCL;
	uint8_t SDA;
} I2C_Config_Pin;


void    I2C_SW_Init(I2C_Config_Pin *I2C);
void    I2C_SW_Start(I2C_Config_Pin *I2C);
void    I2C_SW_Restart(I2C_Config_Pin *I2C);
bool    I2C_SW_Write(I2C_Config_Pin *I2C, uint8_t dat);
uint8_t I2C_SW_Read(I2C_Config_Pin *I2C, uint8_t ack);
void    I2C_SW_Stop(I2C_Config_Pin *I2C);

#endif

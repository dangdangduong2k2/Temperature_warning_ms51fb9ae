#ifndef _I2C_C_
#define _I2C_C_
#include "I2C_SW.h"


void I2C_SW_Init(I2C_Config_Pin *I2C) {
	Config_GPIO(I2C->SCL, Quasi);
	Config_GPIO(I2C->SDA, Quasi);
	Output_Pin(I2C->SCL, 1);
	Output_Pin(I2C->SDA, 1);
}

void I2C_SW_Start(I2C_Config_Pin *I2C) {
	Output_Pin(I2C->SDA, 1);
	Output_Pin(I2C->SCL, 1);
	Config_GPIO(I2C->SDA, Quasi);
	delay_us(10);
	Output_Pin(I2C->SDA, 0);
	delay_us(10);
	Output_Pin(I2C->SCL, 0);
}

void I2C_SW_Restart(I2C_Config_Pin *I2C) {
	delay_us(10);
	Output_Pin(I2C->SDA, 1);
	Output_Pin(I2C->SCL, 1);
	Config_GPIO(I2C->SDA, Quasi);
	delay_us(10);
	Output_Pin(I2C->SDA, 0);
	delay_us(10);
	Output_Pin(I2C->SCL, 0);
}

bool I2C_SW_Write(I2C_Config_Pin *I2C, uint8_t dat) {
	uint8_t i   = 0;
	bool    ack = false;
	Config_GPIO(I2C->SDA, Quasi);
	for (i = 0; i < 8; i++) {
		Output_Pin(I2C->SDA, bit_test(dat, 7 - i));
		delay_us(10);
		Output_Pin(I2C->SCL, 1);
		delay_us(10);
		Output_Pin(I2C->SCL, 0);
		if (i == 7) {
			Config_GPIO(I2C->SDA, Quasi); // Config SDA to input
			Output_Pin(I2C->SDA, 1);
		}
	}
	delay_us(20);
	Output_Pin(I2C->SCL, 1);
	delay_us(25);
	ack = !Input(I2C->SDA);
	delay_us(25);
	Output_Pin(I2C->SCL, 0);
	return ack;
}

uint8_t I2C_SW_Read(I2C_Config_Pin *I2C, uint8_t ack) {
	uint8_t i   = 0;
	uint8_t dat = 0;
	Config_GPIO(I2C->SDA, Quasi); // Config SDA to input
	Output_Pin(I2C->SDA, 1);
	for (i = 0; i < 8; i++) {
		delay_us(10);
		Output_Pin(I2C->SCL, 1);
		delay_us(7);
		dat = dat * 2;
		if (Input(I2C->SDA)) dat = dat + 1;
		delay_us(3);
		Output_Pin(I2C->SCL, 0);
	}
	delay_us(10);
	Config_GPIO(I2C->SDA, Quasi);
	Output_Pin(I2C->SDA, !ack); // send ack
	delay_us(10);
	Output_Pin(I2C->SCL, 1);
	delay_us(50);
	Output_Pin(I2C->SCL, 0);
	return dat;
}

void I2C_SW_Stop(I2C_Config_Pin *I2C) {
	Config_GPIO(I2C->SDA, Quasi);
	Output_Pin(I2C->SDA, 0);
	Output_Pin(I2C->SCL, 0);
	delay_us(10);
	Output_Pin(I2C->SDA, 1);
	delay_us(10);
	Output_Pin(I2C->SCL, 1);
}

#endif

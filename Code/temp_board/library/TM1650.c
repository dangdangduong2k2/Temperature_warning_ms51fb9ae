#ifndef _TM1650_C_
#define _TM1650_C_
#include "TM1650.h"

void TM1650_Set_Config(TM1650_Config *TM) {
	I2C_SW_Start(&TM->I2C);
	I2C_SW_Write(&TM->I2C, 0x48);
	I2C_SW_Write(&TM->I2C, TM->brightness | TM->display);
	I2C_SW_Stop(&TM->I2C);
}

void TM1650_Set_Display(TM1650_Config *TM, bool enable_display) { TM->display = enable_display; }

void TM1650_Set_Bright(TM1650_Config *TM, uint8_t brightness) { TM->brightness = brightness; }

void TM1650_INIT(TM1650_Config *TM) {
	I2C_SW_Init(&TM->I2C);
	TM1650_Set_Config(TM);
}

uint8_t TM1650_Read(TM1650_Config *TM) {
	uint8_t dat = 0;
	I2C_SW_Start(&TM->I2C);
	I2C_SW_Write(&TM->I2C, 0x49);
	dat = I2C_SW_Read(&TM->I2C, 0);
	I2C_SW_Stop(&TM->I2C);
	// if (bit_test(dat, 6) == 0) dat = 0;
	return dat;
}

void TM1650_Write(TM1650_Config *TM, uint8_t addr, uint8_t dat) {
	I2C_SW_Start(&TM->I2C);
	I2C_SW_Write(&TM->I2C, addr);
	I2C_SW_Write(&TM->I2C, dat);
	I2C_SW_Stop(&TM->I2C);
}

#endif

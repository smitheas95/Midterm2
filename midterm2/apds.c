#include <avr/io.h>
#include "i2c_master.h"
#include "apds.h"

void apds_init(){
	uint8_t setup;
	i2c_readReg(APDS_WRITE, APDS9960_ID, &setup,1);
	if(setup != APDS9960_ID_1) while(1);
	setup = 1 << 1 | 1<<0 | 1<<3 | 1<<4;
	i2c_writeReg(APDS_WRITE, APDS9960_ENABLE, &setup, 1);
	setup = DEFAULT_ATIME;
	i2c_writeReg(APDS_WRITE, APDS9960_ATIME, &setup, 1);
	setup = DEFAULT_WTIME;
	i2c_writeReg(APDS_WRITE, APDS9960_WTIME, &setup, 1);
	setup = DEFAULT_PROX_PPULSE;
	i2c_writeReg(APDS_WRITE, APDS9960_PPULSE, &setup, 1);
	setup = DEFAULT_POFFSET_UR;
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_UR, &setup, 1);
	setup = DEFAULT_POFFSET_DL;
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_DL, &setup, 1);
	setup = DEFAULT_CONFIG1;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG1, &setup, 1);
	setup = DEFAULT_PERS;
	i2c_writeReg(APDS_WRITE, APDS9960_PERS, &setup, 1);
	setup = DEFAULT_CONFIG2;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG2, &setup, 1);
	setup = DEFAULT_CONFIG3;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG3, &setup, 1);
}

void readColor(uint16_t *red, uint16_t *green, uint16_t *blue){
	uint8_t redl, redh;
	uint8_t greenl, greenh;
	uint8_t bluel, blueh;
	i2c_readReg(APDS_WRITE, APDS9960_RDATAL, &redl, 1);
	i2c_readReg(APDS_WRITE, APDS9960_RDATAH, &redh, 1);
	i2c_readReg(APDS_WRITE, APDS9960_GDATAL, &greenl, 1);
	i2c_readReg(APDS_WRITE, APDS9960_GDATAH, &greenh, 1);
	i2c_readReg(APDS_WRITE, APDS9960_BDATAL, &bluel, 1);
	i2c_readReg(APDS_WRITE, APDS9960_BDATAH, &blueh, 1);
	*red = redh << 8 | redl;
	*green = greenh << 8 | greenl;
	*blue = blueh << 8 | bluel;
}

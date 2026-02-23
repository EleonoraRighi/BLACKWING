#pragma once


#define I2C_PORT I2C_NUM_0
#define SDA_PIN 23
#define SCL_PIN 22

#define ADXL_ADDR 0x53

#define REG_DEVID       0x00
#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32

#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32


/* API pubblica del componente */

void adxl345_start(void);

//other public functions
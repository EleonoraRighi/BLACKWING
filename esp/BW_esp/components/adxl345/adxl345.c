#include "adxl345.h"
#include "main.h"
#include <stdio.h>
#include "esp_timer.h"
#include "string.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

double s;
double last_s=0;
double delta_s=0;

static void i2c_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}
static esp_err_t write_reg(uint8_t reg, uint8_t val) {
    uint8_t data[2] = {reg, val};
    return i2c_master_write_to_device(
        I2C_PORT, ADXL_ADDR, data, 2, pdMS_TO_TICKS(100));
}
static esp_err_t read_regs(uint8_t reg, uint8_t *buf, size_t len) {
    return i2c_master_write_read_device(
        I2C_PORT, ADXL_ADDR, &reg, 1, buf, len, pdMS_TO_TICKS(100));
}


void adxl345_start(void) {
    i2c_init();

    // accendo il sensore
    write_reg(REG_POWER_CTL, 0x08);

    // formato dati default (±2g)
    write_reg(REG_DATA_FORMAT, 0x00);

    uint8_t raw[6];

    while (1) {
        read_regs(REG_DATAX0, raw, 6);

        int16_t x = (raw[1] << 8) | raw[0];
        int16_t y = (raw[3] << 8) | raw[2];
        //int16_t z = (raw[5] << 8) | raw[4];

        s=sqrt(abs(x)*abs(x)+abs(y)*abs(y));
        delta_s=s-last_s;
        
        //char delta_str[32];
        //snprintf(delta_str, sizeof(delta_str), "%.1f", delta_s);

        if (fabs(delta_s) > 25) {    
            //msg_manager("Hit");
            uart_send_msg("hit\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
        } else if (fabs(delta_s) > 8.5) {
            //msg_manager("Caress");
            
            uart_send_msg("caress\n");
            //uart_send_msg(delta_str);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        
        last_s=s;

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

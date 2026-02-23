#include "angle_test.h"
#include "motor_declaration.h"

#include <stdio.h>
#include "iot_servo.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"
#include "esp_timer.h"  // serve per misurare tempo in microsecondi




uint16_t previous_angle;

 servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 600,
    .max_width_us = 2400,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
        .servo_pin = {
            SERVO_CH0_PIN,
        },
        .ch = {
            LEDC_CHANNEL_0,
        },
    },
    .channel_number = 1,
};

void motor_init(void){
    ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_0,
    .duty_resolution  = LEDC_TIMER_16_BIT,
    .freq_hz          = 50,
    .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    if (iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg) != ESP_OK) {
        printf("servo init failed!\n");
        return;
    }

     iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, STARTING_ANGLE);   
     previous_angle = STARTING_ANGLE;
     printf("Servo initialized to starting angle %d\n", STARTING_ANGLE);

}


void modify_angle(void)
{
    uint16_t angle = previous_angle - 1;

    printf("Modifying angle...\n");
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
    printf("Angle modified to %d\n", angle);
    previous_angle = angle;
    vTaskDelay(pdMS_TO_TICKS(500)); // Attendi 1000 ms per permettere al servo di raggiungere la nuova posizione

}
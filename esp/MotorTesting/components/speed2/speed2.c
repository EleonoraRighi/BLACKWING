
#include "speed2.h"

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


#include "driver/ledc.h"

#define SERVO_PIN 32
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0

void app_main_motor2(void)
{
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ch_conf = {
        .gpio_num = SERVO_PIN,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ch_conf);

    // helper per calcolare duty a partire da microsecondi
    const uint32_t period_us = 1000000 / 50;  // 20,000 us
    const uint32_t max_duty = (1 << 16) - 1;

    // esempio: sposta da 600 µs a 2400 µs subito (massima velocità)
    uint32_t pulse_min = 600;
    uint32_t pulse_max = 1200;

    uint32_t duty_min = (pulse_min * max_duty) / period_us;
    uint32_t duty_max = (pulse_max * max_duty) / period_us;

    TickType_t start = xTaskGetTickCount();
    while((xTaskGetTickCount() - start) < pdMS_TO_TICKS(5000)){ 
        int64_t start_time = esp_timer_get_time(); // microsecondi
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty_min);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(200));
        int64_t end_time = esp_timer_get_time();

        float elapsed_ms = (end_time - start_time) / 1000.0f;
        printf("Tempo impiegato per muoversi di 60°: %.2f ms (%.2f s)\n", elapsed_ms, elapsed_ms / 1000.0f);
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty_max);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    }
}


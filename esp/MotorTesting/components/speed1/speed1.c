
#include "speed1.h"

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


#define SERVO_CH0_PIN 32
#define SERVO_CH1_PIN 32

// MAX SPEED ottenuta è 0.3s/60°    


servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 600,
    .max_width_us = 2400,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
        .servo_pin = {
            SERVO_CH0_PIN,
            SERVO_CH1_PIN,
        },
        .ch = {
            LEDC_CHANNEL_0,
            LEDC_CHANNEL_1,
        },
    },
    .channel_number = 2,
};

void app_main_motor1(void)
{
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


    printf("Moving servo...\n");


    TickType_t start = xTaskGetTickCount();
    while((xTaskGetTickCount() - start) < pdMS_TO_TICKS(5000)){        //ciclo per 5 secondi
        // Muove il servo 0 a 0°, poi a 60°, poi torna a 0° con pause
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(300));


        // --- Misurazione tempo movimento 0° -> 60° ---
        int64_t start_time = esp_timer_get_time(); // microsecondi
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, 60);
        vTaskDelay(pdMS_TO_TICKS(300)); // aspetta completamento movimento
        int64_t end_time = esp_timer_get_time();

        float elapsed_ms = (end_time - start_time) / 1000.0f;
        printf("Tempo impiegato per muoversi di 60°: %.2f ms (%.2f s)\n",
            elapsed_ms, elapsed_ms / 1000.0f);
        // --- Fine misurazione ---

        vTaskDelay(pdMS_TO_TICKS(100));

    }



}

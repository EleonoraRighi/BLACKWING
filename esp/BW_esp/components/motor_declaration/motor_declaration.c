#include <stdio.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

static const char *TAG = "servo";


//servo declarations (open-close wing, tilt wing, rotate head, tilt head)
servo_t servo1 = { .channel = 0, .gpio=18, .min_degree = 5, .max_degree = 100, .min_pulse_us = 500, .max_pulse_us = 2500, .busy=false};
servo_t servo2 = { .channel = 1, .gpio=19, .min_degree = 60, .max_degree = 120, .min_pulse_us = 500, .max_pulse_us = 2500, .busy=false };
servo_t servo3 = { .channel = 2, .gpio=32, .min_degree = 0, .max_degree = 175, .min_pulse_us = 500, .max_pulse_us = 2500, .busy=false };
servo_t servo4 = { .channel = 3, .gpio=33, .min_degree = 10, .max_degree = 100, .min_pulse_us = 500, .max_pulse_us = 2500,.busy=false };



void servo_init_all(void) {

    // TIMER per SERVO 1
    ledc_timer_config_t timer1 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer1);

    // TIMER per SERVO 2 
    ledc_timer_config_t timer2 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer2);

    // TIMER per SERVO 3
    ledc_timer_config_t timer3 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_2,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer3);

    // TIMER per SERVO 4
    ledc_timer_config_t timer4 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_3,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer4);

    // --- CONFIG CANALI ---
    ledc_channel_config_t channels[] = {
        // SERVO 1 usa TIMER 0
        {.gpio_num = servo1.gpio, .channel = servo1.channel, .duty = 0,
         .speed_mode = LEDC_LOW_SPEED_MODE, .hpoint = 0, .timer_sel = LEDC_TIMER_0},

        // SERVO 2 usa TIMER 1 (IMPORTANTE)
        {.gpio_num = servo2.gpio, .channel = servo2.channel, .duty = 0,
         .speed_mode = LEDC_LOW_SPEED_MODE, .hpoint = 0, .timer_sel = LEDC_TIMER_1},

        // SERVO 3 usa TIMER 2
        {.gpio_num = servo3.gpio, .channel = servo3.channel, .duty = 0,
         .speed_mode = LEDC_LOW_SPEED_MODE, .hpoint = 0, .timer_sel = LEDC_TIMER_2},

        // SERVO 4 usa TIMER 3
        {.gpio_num = servo4.gpio, .channel = servo4.channel, .duty = 0,
         .speed_mode = LEDC_LOW_SPEED_MODE, .hpoint = 0, .timer_sel = LEDC_TIMER_3},

    };
    for (int i = 0; i < 4; i++) {
        esp_err_t err = ledc_channel_config(&channels[i]);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "ledc_channel_config failed for channel %d (gpio %d): %d", channels[i].channel, channels[i].gpio_num, err);
        }
    }

    // Stati iniziali 
    servo1.current_angle = DEFAULT_ANGLE_1;
    servo1.target_angle = DEFAULT_ANGLE_1;
    servo1.current_speed = 0;
    servo1.target_speed = 0;
    servo1.running = false;

    servo2.current_angle = DEFAULT_ANGLE_2;
    servo2.target_angle = DEFAULT_ANGLE_2;
    servo2.current_speed = 0;
    servo2.target_speed = 0;
    servo2.running = false;

    servo3.current_angle = DEFAULT_ANGLE_3;
    servo3.target_angle = DEFAULT_ANGLE_3;
    servo3.current_speed = 0;
    servo3.target_speed = 0;
    servo3.running = false;

    servo4.current_angle = DEFAULT_ANGLE_4;
    servo4.target_angle = DEFAULT_ANGLE_4;
    servo4.current_speed = 0;
    servo4.target_speed = 0;
    servo4.running = false;
    


    ESP_LOGI(TAG, "Servo initialized with independent timers");
}

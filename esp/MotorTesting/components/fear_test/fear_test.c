#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/ledc.h"
#include "esp_mac.h"
#include "esp_err.h"
#include <string.h>
#include "fear_test.h"
#include <stdio.h>
#include "driver/ledc.h"

//static const char *TAG = "fear_test";

#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

typedef struct {
    int channel;        // Canale LEDC
    int gpio;           // Pin del servo
    float current_angle;
    float target_angle;
    float current_speed;   // gradi/sec
    float target_speed;    // gradi/sec
    float accel;           // gradi/sec²
    float min_speed;       // gradi/sec
    float min_degree;
    float max_degree;
    bool running;
} servo_t;

// Dichiarazione dei due servo
servo_t servo1 = { .channel = LEDC_CHANNEL_0, .gpio = 18, .current_angle = 0.0f,
                   .min_degree = 0.0f, .max_degree = 180.0f };
servo_t servo2 = { .channel = LEDC_CHANNEL_1, .gpio = 19, .current_angle = 0.0f,
                   .min_degree = 0.0f, .max_degree = 180.0f };

// Converti angolo in duty (10-bit, 50Hz)
uint32_t angle_to_duty(float angle) {
    // range servo reale 0.5ms - 2.5ms per 0°-180°
    float pulse_us = 500 + (angle / 180.0f) * 2000;
    uint32_t duty = (uint32_t)((pulse_us / 20000.0f) * ((1 << 10) - 1));
    return duty;
}

// Inizializzazione LEDC per un servo
void ledc_init_servo(servo_t *s) {
    static bool timer_init = false;

    if (!timer_init) {
        ledc_timer_config_t timer_conf = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_0,
            .duty_resolution = LEDC_TIMER_10_BIT,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&timer_conf);
        timer_init = true;
    }

    ledc_channel_config_t ch_conf = {
        .channel = s->channel,
        .duty = 0,
        .gpio_num = s->gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&ch_conf);
}

// Aggiorna il servo
void servo_update(servo_t *s, float dt) {
    if (!s->running) return;

    float diff = s->target_angle - s->current_angle;
    float direction = (diff > 0.0f) ? 1.0f : -1.0f;

    // Accelerazione/decelerazione
    if (s->current_speed < s->target_speed) s->current_speed += s->accel * dt;
    if (s->current_speed > s->target_speed) s->current_speed -= s->accel * dt;
    if (s->current_speed < s->min_speed) s->current_speed = s->min_speed;

    float move = s->current_speed * dt * direction;
    if (fabsf(move) > fabsf(diff)) move = diff;

    s->current_angle += move;

    if (s->current_angle < s->min_degree) s->current_angle = s->min_degree;
    if (s->current_angle > s->max_degree) s->current_angle = s->max_degree;

    // Aggiorna duty
    uint32_t duty = angle_to_duty(s->current_angle);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, s->channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, s->channel);

    printf("Servo %d angle %.2f -> duty %lu\n", s->channel, s->current_angle, duty);
}

// Task indipendente per ogni servo
void servo_task(void *arg) {
    servo_t *s = (servo_t*) arg;
    const float dt = 0.02f; // 20 ms
    while (1) {
        servo_update(s, dt);
        vTaskDelay(pdMS_TO_TICKS((int)(dt*1000)));
    }
}

// Main
void fear_init(void) {
    printf("Starting servo demo\n");

    // Inizializza servo
    ledc_init_servo(&servo1);
    ledc_init_servo(&servo2);

    servo1.running = true;
    servo1.target_speed = 300.0f;  // gradi/sec
    servo1.accel = 360.0f;         // gradi/sec²
    servo1.min_speed = 20.0f;

    servo2.running = true;
    servo2.target_speed = 60.0f;   
    servo2.accel = 180.0f;
    servo2.min_speed = 10.0f;

    // Crea task
    xTaskCreate(servo_task, "servo1_task", 2048, &servo1, 5, NULL);
    xTaskCreate(servo_task, "servo2_task", 2048, &servo2, 5, NULL);

    // Ciclo demo: servo1 da 0 a 180, servo2 da 180 a 0
    while (1) {
        servo1.target_angle = 140.0f;
        servo2.target_angle = 60.0f;
        vTaskDelay(pdMS_TO_TICKS(5000));

        servo1.target_angle = 40.0f;
        servo2.target_angle = 120.0f;
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

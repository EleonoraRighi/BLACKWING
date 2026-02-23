#include <stdio.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"  // <- qui definiamo tutte le sequenze

static const char *TAG = "servo";

// Mutex per proteggere la variabile globale emotion_running
SemaphoreHandle_t emotion_mutex;

// Struttura per passare dati al task generico
typedef struct {
    servo_t *servo;
    const int *seq;
    int len;
} servo_task_args_t;

//prototypes
void servo_task(void *arg);
void servo_update(servo_t *s, float dt);
void servo_sequence_task(void *arg);
void servo_set_angle_hw(servo_t *s, float angle);
void run_servo_sequence(servo_t *s, const int *seq, int len);

void servo_manager_init(void){
    // Creazione mutex
    emotion_mutex = xSemaphoreCreateMutex();

    // Creazione task base che aggiorna il servo (movimento fluido)
    xTaskCreate(servo_task, "servo1_base_task", 4096, &servo1, 5, NULL);
    xTaskCreate(servo_task, "servo2_base_task", 4096, &servo2, 5, NULL);
    xTaskCreate(servo_task, "servo3_base_task", 4096, &servo3, 5, NULL);
    xTaskCreate(servo_task, "servo4_base_task", 4096, &servo4, 5, NULL);

    emotion_running = false;
/*
    vTaskDelay(pdMS_TO_TICKS(1000)); // lascia il tempo ai task di partire

    //go to default position at init
    servo1.target_speed = 100;
    servo1.accel = 25;
    servo1.decel = 25;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 100;
    servo2.accel = 25;
    servo2.decel = 25;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 25;
    servo3.decel = 25;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 100;
    servo4.accel = 25;
    servo4.decel = 25;
    servo4.min_speed = 100;
    servo4.running = true;

    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
*/
}

// --- task che aggiorna movimento fluido ---
void servo_task(void *arg)
{
    servo_t *s = (servo_t *)arg;
    const float dt = 0.02f;   // sempre 20 ms
    const TickType_t xDelay = pdMS_TO_TICKS(20);

    while (1) {
        if (s->running) {
            servo_update(s, dt);
        }
        vTaskDelay(xDelay);
    }
/*  servo_t *s = (servo_t *)arg;
    const float dt = 0.02f;

    while (1) {
        if (s->running) {
            servo_update(s, dt);
            vTaskDelay(pdMS_TO_TICKS(20));
        } else {
            // Quando il servo non sta muovendo, dormi più a lungo per ridurre CPU/log
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
*/  }

void servo_update(servo_t *s, float dt)
{
    if (!s->running) return;

    float diff = s->target_angle - s->current_angle;
    float abs_diff = fabsf(diff);
    float direction = (diff > 0) ? 1.0f : -1.0f;

    if (s->current_speed < s->target_speed) s->current_speed += s->accel * dt;
    else if (s->current_speed > s->target_speed) s->current_speed -= s->accel * dt;
    if (s->current_speed < s->min_speed) s->current_speed = s->min_speed;

    float move = s->current_speed * dt * direction;
    if (fabsf(move) > abs_diff) move = abs_diff * direction;

    s->current_angle += move;

    if (s->current_angle < s->min_degree) s->current_angle = s->min_degree;
    if (s->current_angle > s->max_degree) s->current_angle = s->max_degree;

    servo_set_angle_hw(s, s->current_angle);
}

void servo_set_angle_hw(servo_t *s, float angle)
{
    // Non fare nulla se il servo è fermo
    if (!s->running) return;

    if (angle < s->min_degree) angle = s->min_degree;
    if (angle > s->max_degree) angle = s->max_degree;

    float norm = (s->max_degree - s->min_degree > 0.0f) ?
                 (angle - s->min_degree) / (s->max_degree - s->min_degree) : 0.0f;

    float pulse_us = s->min_pulse_us + norm * (s->max_pulse_us - s->min_pulse_us);
    uint32_t duty = (uint32_t)((pulse_us / 20000.0f) * ((1 << 10) - 1));

    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)s->channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)s->channel);

    //ESP_LOGI(TAG, "servo gpio %d ch %d angle %.2f -> pulse_us %.1f duty %lu", s->gpio, s->channel, angle, pulse_us, duty);
}

static inline int boundary_verify(servo_t *s, int target) {
    if (target < s->min_degree) return s->min_degree;
    if (target > s->max_degree) return s->max_degree;
    return target;
}


// --- task generico che esegue una sequenza ---
void servo_sequence_task(void *arg)
{
    servo_task_args_t *args = (servo_task_args_t *)arg;
    servo_t *s = args->servo;

    s->running = true;

    const TickType_t max_wait_ticks = pdMS_TO_TICKS(2000);  // es: 2 secondi per step
    const float stall_threshold = 0.1f;
    const TickType_t stall_check_period = pdMS_TO_TICKS(100);

    for (int i = 0; i < args->len; i++) {
        // clamp dell'angolo prima di assegnare il target
        s->target_angle = boundary_verify(s, args->seq[i]);

        TickType_t start_tick = xTaskGetTickCount();
        float last_angle = s->current_angle;

        while (1) {
            if (fabsf(s->current_angle - s->target_angle) <= 1.0f) break;

            TickType_t now = xTaskGetTickCount();
            if ((now - start_tick) > max_wait_ticks) {
                ESP_LOGW(TAG, "servo ch %d: timeout su step %d, angolo target %.2f, angolo attuale %.2f",
                         s->channel, i, s->target_angle, s->current_angle);
                break;
            }

            vTaskDelay(stall_check_period);
            float current = s->current_angle;
            if (fabsf(current - last_angle) < stall_threshold) {
                ESP_LOGW(TAG, "servo ch %d: stall rilevato su step %d, angolo non cambia (%.2f)",
                         s->channel, i, current);
                break;
            }
            last_angle = current;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

/*    s->running = false;

    if (xSemaphoreTake(emotion_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (!servo1.running && !servo2.running && !servo3.running && !servo4.running) {
            emotion_running = false;
        }
        xSemaphoreGive(emotion_mutex);
    }
*/

    s->busy = false;  // segna che la sequenza è finita

    // Controlla se TUTTI i servo hanno finito
    if (xSemaphoreTake(emotion_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (!servo1.busy && !servo2.busy && !servo3.busy && !servo4.busy) {
            emotion_running = false;  // ora l'emozione è conclusa
        }
        xSemaphoreGive(emotion_mutex);
    }


    vPortFree(arg);
    vTaskDelete(NULL);
}



// --- funzione helper per avviare una sequenza su un servo ---
void run_servo_sequence(servo_t *s, const int *seq, int len)
{
    if (s->busy) return; // se già in sequenza, ignoriamo
    s->busy = true;      // segna la sequenza attiva
    //xTaskCreate(servo_sequence_task, "servo_seq", 4096, (void*)s, 2, NULL);
    
    servo_task_args_t *args = pvPortMalloc(sizeof(servo_task_args_t));
    args->servo = s;
    args->seq = seq;
    args->len = len;

    char name[16];
    snprintf(name, sizeof(name), "seq_task_%d", s->channel);
    xTaskCreate(servo_sequence_task, name, 4096, args, 5, NULL);
}

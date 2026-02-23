#pragma once 
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"

// Default angles for servos at initialization
#define DEFAULT_ANGLE_1  40.0f
#define DEFAULT_ANGLE_2  85.0f  
#define DEFAULT_ANGLE_3  90.0f
#define DEFAULT_ANGLE_4  40.0f



// Struttura che descrive un singolo servo
typedef struct {
    int gpio;
    int channel;

    float current_angle;
    float target_angle;

    float current_speed;
    float target_speed;
    bool running;

    // Dinamica
    float accel;     
    float decel;     
    float min_speed; 

    // Limiti fisici e PWM
    float min_degree;
    float max_degree;
    uint32_t min_pulse_us;
    uint32_t max_pulse_us;
    float overshoot;        // per animazioni elastiche
    float stop_brake;       // frenata finale

    bool busy; // nuovo flag per indicare sequenza in esecuzione

} servo_t;


// Dichiarazioni dei due servo condivisi
extern servo_t servo1;
extern servo_t servo2;
extern servo_t servo3;
extern servo_t servo4;

//timer globali 
extern esp_timer_handle_t servo1_timer;
extern esp_timer_handle_t servo2_timer;
extern esp_timer_handle_t servo3_timer;
extern esp_timer_handle_t servo4_timer;

extern bool emotion_running;
extern volatile bool idle_running;
extern volatile bool idle_abort_requested;

// Funzioni comuni
void servo_init_all(void);         // inizializza entrambi i servo
void servo_update(servo_t *s, float dt);
void servo_task(void *arg);
void servo_set_angle_hw(servo_t *s, float angle);

void servo_manager_init(void);
void servo_manager(void);
void run_servo_sequence(servo_t *servo, const int *sequence, int length);




#define SERVO1_UPDATE_MS      5

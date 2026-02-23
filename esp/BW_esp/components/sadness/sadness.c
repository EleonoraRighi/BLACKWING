#include "sadness.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

static const char *TAG = "sadness";
static char previous_action[16];


void pensiveness(void){
    uart_send_msg("\033[35mPensiveness triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 5;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 1;
    servo1.running = true;

    servo2.target_speed = 5;
    servo2.accel = 1;
    servo2.decel = 1;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed =50;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 50;
    servo4.decel = 25;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, pensiveness_seq1, (sizeof(pensiveness_seq1)/sizeof(pensiveness_seq1[0])));
    run_servo_sequence(&servo2, pensiveness_seq2, (sizeof(pensiveness_seq2)/sizeof(pensiveness_seq2[0])));
    run_servo_sequence(&servo3, pensiveness_seq3, (sizeof(pensiveness_seq3)/sizeof(pensiveness_seq3[0])));
    run_servo_sequence(&servo4, pensiveness_seq4, (sizeof(pensiveness_seq4)/sizeof(pensiveness_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 300;
    servo1.accel = 100;
    servo1.decel = 50;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 50;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void sadness(void){
    uart_send_msg("\033[35mSadness triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 10;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 1;
    servo1.running = true;

    servo2.target_speed = 5;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed = 50;
    servo3.accel = 25;
    servo3.decel = 25;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, sadness_seq1, (sizeof(sadness_seq1)/sizeof(sadness_seq1[0])));
    run_servo_sequence(&servo2, sadness_seq2, (sizeof(sadness_seq2)/sizeof(sadness_seq2[0])));
    run_servo_sequence(&servo3, sadness_seq3, (sizeof(sadness_seq3)/sizeof(sadness_seq3[0])));
    run_servo_sequence(&servo4, sadness_seq4, (sizeof(sadness_seq4)/sizeof(sadness_seq4[0])));

    vTaskDelay(pdMS_TO_TICKS(7000));
    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 10;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 5;
    servo1.running = true;

    servo2.target_speed = 50;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed =100;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 10;
    servo3.running = true;
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void grief(void){
    uart_send_msg("\033[35mGrief triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 900;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed = 200;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 900;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, grief_seq1, (sizeof(grief_seq1)/sizeof(grief_seq1[0])));
    run_servo_sequence(&servo2, grief_seq2, (sizeof(grief_seq2)/sizeof(grief_seq2[0])));
    run_servo_sequence(&servo3, grief_seq3, (sizeof(grief_seq3)/sizeof(grief_seq3[0])));
    run_servo_sequence(&servo4, grief_seq4, (sizeof(grief_seq4)/sizeof(grief_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 10;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 5;
    servo1.running = true;

    servo2.target_speed = 50;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
 }
#include "disgust.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

static const char *TAG = "disgust";
static char previous_action[16];


void boredom(void){
    uart_send_msg("\033[35mBoredom triggered\033[0m\n");
        if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 50;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 10;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed = 50;
    servo3.accel = 25;
    servo3.decel = 25;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 80;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 10;
    servo4.running = true;
    

    run_servo_sequence(&servo1, boredom_seq1, (sizeof(boredom_seq1)/sizeof(boredom_seq1[0])));
    run_servo_sequence(&servo2, boredom_seq2, (sizeof(boredom_seq2)/sizeof(boredom_seq2[0])));
    run_servo_sequence(&servo3, boredom_seq3, (sizeof(boredom_seq3)/sizeof(boredom_seq3[0])));
    run_servo_sequence(&servo4, boredom_seq4, (sizeof(boredom_seq4)/sizeof(boredom_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 200;
    servo1.accel = 50;
    servo1.decel = 50;
    servo1.min_speed = 100;
    servo1.running = true;
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void disgust(void){
    uart_send_msg("\033[35mDisgust triggered\033[0m\n");
        if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 900;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 800;
    servo1.running = true;

    servo2.target_speed = 900;
    servo2.accel = 100;
    servo2.decel = 10;
    servo2.min_speed = 800;
    servo2.running = true;

    servo3.target_speed = 900;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 800;
    servo3.running = true;

    servo4.target_speed = 900;
    servo4.accel = 100;
    servo4.decel = 100;
    servo4.min_speed = 800;
    servo4.running = true;

    

    run_servo_sequence(&servo1, disgust_seq1, (sizeof(disgust_seq1)/sizeof(disgust_seq1[0])));
    run_servo_sequence(&servo2, disgust_seq2, (sizeof(disgust_seq2)/sizeof(disgust_seq2[0])));
    run_servo_sequence(&servo3, disgust_seq3, (sizeof(disgust_seq3)/sizeof(disgust_seq3[0])));
    run_servo_sequence(&servo4, disgust_seq4, (sizeof(disgust_seq4)/sizeof(disgust_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 100;
    servo1.accel = 20;
    servo1.decel = 20;
    servo1.min_speed = 50;
    servo1.running = true;
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void loathing(void){
    uart_send_msg("\033[35mLoathing triggered\033[0m\n");
        if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 400;
    servo1.accel = 50;
    servo1.decel = 50;
    servo1.min_speed = 0;
    servo1.running = true;

    servo2.target_speed = 400;
    servo2.accel = 50;
    servo2.decel = 50;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 200;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 100;
    servo4.running = true;
    

    run_servo_sequence(&servo1, loathing_seq1, (sizeof(loathing_seq1)/sizeof(loathing_seq1[0])));
    run_servo_sequence(&servo2, loathing_seq2, (sizeof(loathing_seq2)/sizeof(loathing_seq2[0])));
    run_servo_sequence(&servo3, loathing_seq3, (sizeof(loathing_seq3)/sizeof(loathing_seq3[0])));
    run_servo_sequence(&servo4, loathing_seq4, (sizeof(loathing_seq4)/sizeof(loathing_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 200;
    servo1.accel = 20;
    servo1.decel = 20;
    servo1.min_speed = 100;
    servo1.running = true;
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}   
#include "joy.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"


static const char *TAG = "joy";

void serenity(void){
    uart_send_msg("\033[35mSerenity triggered\033[0m\n");
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

    servo2.target_speed = 10;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed = 200;
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 900;
    servo4.accel = 100;
    servo4.decel = 100;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, serenity_seq1, (sizeof(serenity_seq1)/sizeof(serenity_seq1[0])));
    run_servo_sequence(&servo2, serenity_seq2, (sizeof(serenity_seq2)/sizeof(serenity_seq2[0])));
    run_servo_sequence(&servo3, serenity_seq3, (sizeof(serenity_seq3)/sizeof(serenity_seq3[0])));
    run_servo_sequence(&servo4, serenity_seq4, (sizeof(serenity_seq4)/sizeof(serenity_seq4[0])));

    
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
    //uart_send_msg("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void joy(void){
    uart_send_msg("\033[35mJoy triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 500;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 30;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 100;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 50;
    servo4.running = true;

    

    run_servo_sequence(&servo1, joy_seq1, (sizeof(joy_seq1)/sizeof(joy_seq1[0])));
    run_servo_sequence(&servo2, joy_seq2, (sizeof(joy_seq2)/sizeof(joy_seq2[0])));
    run_servo_sequence(&servo3, joy_seq3, (sizeof(joy_seq3)/sizeof(joy_seq3[0])));
    run_servo_sequence(&servo4, joy_seq4, (sizeof(joy_seq4)/sizeof(joy_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 100;
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
void ecstasy(void){
    uart_send_msg("\033[35mEcstasy triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 600;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 30;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 100;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 50;
    servo4.running = true;

    

    run_servo_sequence(&servo1, ecstasy_seq1, (sizeof(ecstasy_seq1)/sizeof(ecstasy_seq1[0])));
    run_servo_sequence(&servo2, ecstasy_seq2, (sizeof(ecstasy_seq2)/sizeof(ecstasy_seq2[0])));
    run_servo_sequence(&servo3, ecstasy_seq3, (sizeof(ecstasy_seq3)/sizeof(ecstasy_seq3[0])));
    run_servo_sequence(&servo4, ecstasy_seq4, (sizeof(ecstasy_seq4)/sizeof(ecstasy_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 100;
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
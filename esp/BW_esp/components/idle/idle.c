#include "idle.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

volatile bool idle_running = false;
volatile bool idle_abort_requested = false;

void idle_1(void){
    printf("\033[35mIdle_1 animation started!\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running - skipping\n");
        return;
    }

    idle_abort_requested = false;
    idle_running = true;
    emotion_running = true;

    servo1.target_speed = 10;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 5;
    servo1.running = true;

    servo2.target_speed = 5;
    servo2.accel = 1;
    servo2.decel = 1;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed = 5;
    servo3.accel = 5;
    servo3.decel = 5;
    servo3.min_speed = 1;
    servo3.running = true;

    servo4.target_speed = 5;
    servo4.accel = 5;
    servo4.decel = 5;
    servo4.min_speed = 1;
    servo4.running = true;

    

    run_servo_sequence(&servo1, idle1_seq1, (sizeof(idle1_seq1)/sizeof(idle1_seq1[0])));
    run_servo_sequence(&servo2, idle1_seq2, (sizeof(idle1_seq2)/sizeof(idle1_seq2[0])));
    run_servo_sequence(&servo3, idle1_seq3, (sizeof(idle1_seq3)/sizeof(idle1_seq3[0])));
    run_servo_sequence(&servo4, idle1_seq4, (sizeof(idle1_seq4)/sizeof(idle1_seq4[0])));

}
void idle_2(void){
    uart_send_msg("\033[35mIdle_2 animation started!\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running - skipping\n");
        return;
    }

    idle_abort_requested = false;
    idle_running = true;
    emotion_running = true;


    servo1.target_speed = 500;
    servo1.accel = 25;
    servo1.decel = 25;
    servo1.min_speed = 200;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 5;
    servo2.running = true;

    servo3.target_speed = 10;
    servo3.accel = 25;
    servo3.decel = 25;
    servo3.min_speed = 5;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, idle2_seq1, (sizeof(idle2_seq1)/sizeof(idle2_seq1[0])));
    run_servo_sequence(&servo2, idle2_seq2, (sizeof(idle2_seq2)/sizeof(idle2_seq2[0])));
    run_servo_sequence(&servo3, idle2_seq3, (sizeof(idle2_seq3)/sizeof(idle2_seq3[0])));
    run_servo_sequence(&servo4, idle2_seq4, (sizeof(idle2_seq4)/sizeof(idle2_seq4[0])));


   

    //end of animation
   
}
void idle_3(void){
    uart_send_msg("\033[35mIdle_3 animation started!\033[0m\n");

    servo1.target_speed = 100;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 50;
    servo1.running = true;

    servo2.target_speed = 100;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 50;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 50;
    servo3.running = true;

    servo4.target_speed = 100;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 50;
    servo4.running = true;

    

    run_servo_sequence(&servo1, idle3_seq1, (sizeof(idle3_seq1)/sizeof(idle3_seq1[0])));
    run_servo_sequence(&servo2, idle3_seq2, (sizeof(idle3_seq2)/sizeof(idle3_seq2[0])));
    run_servo_sequence(&servo3, idle3_seq3, (sizeof(idle3_seq3)/sizeof(idle3_seq3[0])));
    run_servo_sequence(&servo4, idle3_seq4, (sizeof(idle3_seq4)/sizeof(idle3_seq4[0])));

    
    //end of animation

}

void bow(void){
    uart_send_msg("\033[35mBOW animation started!\033[0m\n");

    servo1.target_speed = 50;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 10;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 5;
    servo2.running = true;

    servo3.target_speed = 50;
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 5;
    servo4.running = true;

    

    run_servo_sequence(&servo1, bow_seq1, (sizeof(bow_seq1)/sizeof(bow_seq1[0])));
    run_servo_sequence(&servo2, bow_seq2, (sizeof(bow_seq2)/sizeof(bow_seq2[0])));
    run_servo_sequence(&servo3, bow_seq3, (sizeof(bow_seq3)/sizeof(bow_seq3[0])));
    run_servo_sequence(&servo4, bow_seq4, (sizeof(bow_seq4)/sizeof(bow_seq4[0])));

    
    //end of animation

}

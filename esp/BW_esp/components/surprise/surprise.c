#include "surprise.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"


static const char *TAG = "surprise";
static char previous_action[16]="";




void distraction(void){
    uart_send_msg("\033[35mDistraction function triggered\033[0m\n");

    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


    servo1.target_speed = 2000;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 800;
    servo1.running = true;

    servo2.target_speed = 600;
    servo2.accel = 100;
    servo2.decel = 100;
    servo2.min_speed = 200;
    servo2.running = true;

    servo3.target_speed = 700;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 300;
    servo3.running = true;

    servo4.target_speed = 300;
    servo4.accel = 100;
    servo4.decel = 100;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, distraction_seq1, (sizeof(distraction_seq1)/sizeof(distraction_seq1[0])));
    run_servo_sequence(&servo2, distraction_seq2, (sizeof(distraction_seq2)/sizeof(distraction_seq2[0])));
    run_servo_sequence(&servo3, distraction_seq3, (sizeof(distraction_seq3)/sizeof(distraction_seq3[0])));
    run_servo_sequence(&servo4, distraction_seq4, (sizeof(distraction_seq4)/sizeof(distraction_seq4[0])));

    
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

    servo2.target_speed = 10;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 5;
    servo2.running = true;

    //uart_send_msg("Distraction animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void surprise(void){    
    uart_send_msg("\033[35mSurprise function triggered\033[0m\n");
        if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


    servo1.target_speed = 1000;
    servo1.accel = 50;
    servo1.decel = 50;
    servo1.min_speed = 800;
    servo1.running = true;

    servo2.target_speed = 300;
    servo2.accel = 100;
    servo2.decel = 100;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 900;
    servo3.accel = 120;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 200;
    servo4.accel = 50;
    servo4.decel = 100;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, surprise_seq1, (sizeof(surprise_seq1)/sizeof(surprise_seq1[0])));
    run_servo_sequence(&servo2, surprise_seq2, (sizeof(surprise_seq2)/sizeof(surprise_seq2[0])));
    run_servo_sequence(&servo3, surprise_seq3, (sizeof(surprise_seq3)/sizeof(surprise_seq3[0])));
    run_servo_sequence(&servo4, surprise_seq4, (sizeof(surprise_seq4)/sizeof(surprise_seq4[0])));

    
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

    //uart_send_msg("Distraction animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void amazement(void){
    uart_send_msg("\033[35mAmazement function triggered\033[0m\n");
            if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


   servo1.target_speed = 30;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 10;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 10;
    servo4.running = true;


    

    run_servo_sequence(&servo1, amazement_seq1, (sizeof(amazement_seq1)/sizeof(amazement_seq1[0])));
    run_servo_sequence(&servo2, amazement_seq2, (sizeof(amazement_seq2)/sizeof(amazement_seq2[0])));
    run_servo_sequence(&servo3, amazement_seq3, (sizeof(amazement_seq3)/sizeof(amazement_seq3[0])));
    run_servo_sequence(&servo4, amazement_seq4, (sizeof(amazement_seq4)/sizeof(amazement_seq4[0])));

    
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

    //uart_send_msg("Distraction animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
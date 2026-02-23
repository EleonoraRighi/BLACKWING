#include "anger.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"


void annoyance(void){
    printf("\033[35mAnnoyance animation started!\033[0m\n");

    if (emotion_running) {
        printf("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


    servo1.target_speed = 50;
    servo1.accel = 5;
    servo1.decel = 5;
    servo1.min_speed = 10;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 50;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 25;
    servo4.decel = 25;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, annoyance_seq1, (sizeof(annoyance_seq1)/sizeof(annoyance_seq1[0])));
    run_servo_sequence(&servo2, annoyance_seq2, (sizeof(annoyance_seq2)/sizeof(annoyance_seq2[0])));
    run_servo_sequence(&servo3, annoyance_seq3, (sizeof(annoyance_seq3)/sizeof(annoyance_seq3[0])));
    run_servo_sequence(&servo4, annoyance_seq4, (sizeof(annoyance_seq4)/sizeof(annoyance_seq4[0])));

    
    
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

    servo3.target_speed = 100;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 50;
    servo3.running = true;
    //uart_send_msg("Annoyance animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void anger(void){
    uart_send_msg("\033[35mAnger animation started!\033[0m\n");
        if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 300;
    servo1.accel = 50;
    servo1.decel = 50;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 100;
    servo2.accel = 50;
    servo2.decel = 50;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 5;
    servo3.decel = 5;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 400;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, anger_seq1, (sizeof(anger_seq1)/sizeof(anger_seq1[0])));
    run_servo_sequence(&servo2, anger_seq2, (sizeof(anger_seq2)/sizeof(anger_seq2[0])));
    run_servo_sequence(&servo3, anger_seq3, (sizeof(anger_seq3)/sizeof(anger_seq3[0])));
    run_servo_sequence(&servo4, anger_seq4, (sizeof(anger_seq4)/sizeof(anger_seq4[0])));


   

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
    //uart_send_msg("Anger animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void rage(void){
    uart_send_msg("\033[35mRage animation started!\033[0m\n");
        if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 1000;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 300;
    servo2.accel = 50;
    servo2.decel = 50;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 800;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 80;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, rage_seq1, (sizeof(rage_seq1)/sizeof(rage_seq1[0])));
    run_servo_sequence(&servo2, rage_seq2, (sizeof(rage_seq2)/sizeof(rage_seq2[0])));
    run_servo_sequence(&servo3, rage_seq3, (sizeof(rage_seq3)/sizeof(rage_seq3[0])));
    run_servo_sequence(&servo4, rage_seq4, (sizeof(rage_seq4)/sizeof(rage_seq4[0])));

    
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
    //uart_send_msg("Rage animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);

}

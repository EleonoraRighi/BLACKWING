#include "anticipation.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"


static const char *TAG = "anticipation";
static char previous_action[16];


void interest(void){
    uart_send_msg("\033[35mInterest triggered!\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 300;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 100;
    servo2.accel = 100;
    servo2.decel = 100;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 200;
    servo3.accel = 50;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 500;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, interest_seq1, (sizeof(interest_seq1)/sizeof(interest_seq1[0])));
    run_servo_sequence(&servo2, interest_seq2, (sizeof(interest_seq2)/sizeof(interest_seq2[0])));
    run_servo_sequence(&servo3, interest_seq3, (sizeof(interest_seq3)/sizeof(interest_seq3[0])));
    run_servo_sequence(&servo4, interest_seq4, (sizeof(interest_seq4)/sizeof(interest_seq4[0])));

    
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
void anticipation(void){
    uart_send_msg("\033[35mAnticipation triggered!\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 300;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 100;
    servo2.accel = 100;
    servo2.decel = 100;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 200;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 500;
    servo4.accel = 100;
    servo4.decel = 100;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, anticipation_seq1, (sizeof(anticipation_seq1)/sizeof(anticipation_seq1[0])));
    run_servo_sequence(&servo2, anticipation_seq2, (sizeof(anticipation_seq2)/sizeof(anticipation_seq2[0])));
    run_servo_sequence(&servo3, anticipation_seq3, (sizeof(anticipation_seq3)/sizeof(anticipation_seq3[0])));
    run_servo_sequence(&servo4, anticipation_seq4, (sizeof(anticipation_seq4)/sizeof(anticipation_seq4[0])));

    
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

void vigilance(void){
    uart_send_msg("\033[35mVigilance triggered!\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;
    servo1.target_speed = 300;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 100;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 200;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 500;
    servo4.accel = 100;
    servo4.decel = 100;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, vigilance_seq1, (sizeof(vigilance_seq1)/sizeof(vigilance_seq1[0])));
    run_servo_sequence(&servo2, vigilance_seq2, (sizeof(vigilance_seq2)/sizeof(vigilance_seq2[0])));
    run_servo_sequence(&servo3, vigilance_seq3, (sizeof(vigilance_seq3)/sizeof(vigilance_seq3[0])));
    run_servo_sequence(&servo4, vigilance_seq4, (sizeof(vigilance_seq4)/sizeof(vigilance_seq4[0])));

    
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
#include "fear.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

static const char *TAG = "fear";
bool emotion_running = false;


void apprehension (void){
    uart_send_msg("\033[35mApprehension triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 100;
    servo1.accel = 20;
    servo1.decel = 20;
    servo1.min_speed = 50;
    servo1.running = true;

    servo2.target_speed = 200;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 900;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 600;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, apprehension_seq1, (sizeof(apprehension_seq1)/sizeof(apprehension_seq1[0])));
    run_servo_sequence(&servo2, apprehension_seq2, (sizeof(apprehension_seq2)/sizeof(apprehension_seq2[0])));
    run_servo_sequence(&servo3, apprehension_seq3, (sizeof(apprehension_seq3)/sizeof(apprehension_seq3[0])));
    run_servo_sequence(&servo4, apprehension_seq4, (sizeof(apprehension_seq4)/sizeof(apprehension_seq4[0])));

    
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
void fear(void){
    uart_send_msg("\033[35mFear triggered\033[0m\n");
    if (emotion_running) {
        uart_send_msg("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 1000;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 500;
    servo1.running = true;

    servo2.target_speed = 300;
    servo2.accel = 50;
    servo2.decel = 50;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 400;
    servo3.accel = 100;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 600;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 100;
    servo4.running = true;

    

    run_servo_sequence(&servo1, fear_seq1, (sizeof(fear_seq1)/sizeof(fear_seq1[0])));
    run_servo_sequence(&servo2, fear_seq2, (sizeof(fear_seq2)/sizeof(fear_seq2[0])));
    run_servo_sequence(&servo3, fear_seq3, (sizeof(fear_seq3)/sizeof(fear_seq3[0])));
    run_servo_sequence(&servo4, fear_seq4, (sizeof(fear_seq4)/sizeof(fear_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // adesso lancio le default sequence
    servo1.target_speed = 200;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 100;
    servo1.running = true;
    //uart_send_msg("Fear animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}   


void terror(void)
{
    if (emotion_running) {
        uart_send_msg("Terror already running — skipping\n");
        return;
    }
    emotion_running = true;

    uart_send_msg("\033[35mTerror triggered\033[0m\n");

    servo1.target_speed = 1000;
    servo1.accel = 100;
    servo1.decel = 100;
    servo1.min_speed = 100;
    servo1.running = true;

    servo2.target_speed = 400;
    servo2.accel = 50;
    servo2.decel = 50;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 500;
    servo3.accel = 50;
    servo3.decel = 100;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 100;
    servo4.accel = 50;
    servo4.decel = 10;
    servo4.min_speed = 90;
    servo4.running = true;

    

    run_servo_sequence(&servo1, terror_seq1, (sizeof(terror_seq1)/sizeof(terror_seq1[0])));
    run_servo_sequence(&servo2, terror_seq2, (sizeof(terror_seq2)/sizeof(terror_seq2[0])));
    run_servo_sequence(&servo3, terror_seq3, (sizeof(terror_seq3)/sizeof(terror_seq3[0])));
    run_servo_sequence(&servo4, terror_seq4, (sizeof(terror_seq4)/sizeof(terror_seq4[0])));

    
    //end of animation
    // aspetta che tutte le sequenze finiscano
    while(emotion_running) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    // adesso lancio le default sequence
    servo1.target_speed = 50;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 10;
    servo1.running = true;

    servo4.target_speed = 100;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 50;
    servo4.running = true;
    //uart_send_msg("Terror animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    

}




/*
void terror(void)
{
    printf("\033[35mTerror triggered\033[0m\n");
    ESP_LOGI(TAG, "AVVIO ANIMAZIONE PAURA");

    static const int seq1[] = {0, 90, 180};
    static const int terror_seq2[] = {0, 180};
    #define SEQ1_LEN (sizeof(seq1)/sizeof(seq1[0]))
    #define SEQ2_LEN (sizeof(terror_seq2)/sizeof(terror_seq2[0]))

    servo1.target_speed = 900.0f; servo1.accel = 100.0f; servo1.decel = 100.0f; servo1.min_speed = 800.0f;
    servo2.target_speed = 100.0f; servo2.accel = 5.0f;  servo2.decel = 5.0f;  servo2.min_speed = 50.0f;

    servo1.running = true;
    servo2.running = true;

    int idx1 = 0, idx2 = 0;
    int64_t start = esp_timer_get_time();
    int64_t last_step = start;

    while ((esp_timer_get_time() - start) < 5000000) // 5 secondi
    {
        int64_t now = esp_timer_get_time();

        // aggiorna target ogni 500ms SENZA bloccare nulla
        if (now - last_step >= 1500000){ // 1.5s
            last_step = now;

            servo1.target_angle = seq1[idx1];
            servo2.target_angle = terror_seq2[idx2];

            idx1 = (idx1 + 1) % SEQ1_LEN;
            idx2 = (idx2 + 1) % SEQ2_LEN;

            printf("idx1: %d, idx2: %d\n", idx1, idx2);
        }

        vTaskDelay(1); // permette al task di respirare senza bloccare nulla
    }

    servo1.target_angle = 90.0f;
    servo2.target_angle = 90.0f;
    servo1.running = false;
    servo2.running = false;

    ESP_LOGI(TAG, "Animazione terminata dopo 5 secondi");
}*/
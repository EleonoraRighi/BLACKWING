#include "anger.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "math.h"
#include "thresholds.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include "driver/ledc.h"
#include "esp_timer.h"



static const char *TAG = "anger";
static char previous_action[16];
static float beta = 0;
static float inclination;
static float alpha;
static bool increase;
static float x;
float (*f)(float);    //chosen function (depending on S or N)
static float fS(float x) {
    return (0.1*expf(x));      // linear
}
static float fN(float x) {
    return (0.05*expf(x));      // exponential
}
float derivative(float (*f)(float), float x) {
    float h = 0.0001;
    return (f(x + h) - f(x)) / h;
}


//void mood_manager(char mood, bool increase, float value){}

esp_err_t anger_init(void)
{
    ESP_LOGI(TAG, "anger init");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.2;
    else if (personality[3] == 'P') alpha = 0.3;
    else ESP_LOGW(TAG, "Errore nel calcolo di alpha");

    if (personality[1] == 'S') f=fS;
    else if (personality[1] == 'N') f=fN;
    else ESP_LOGW(TAG, "Errore nel calcolo di inclination");
    //printf("\nAnger parameters are initialized\n");
    return ESP_OK;
}

void anger_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        beta=0;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con anger nel calcolo di gamma");
    }

    inclination = derivative(f,beta);
    printf("\ninclination= %.2f", inclination);


    if(strcmp("hug",action_name)==0||strcmp("caress",action_name)==0||strcmp("sorrow",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[0]+=( emotion[0]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+inclination));
        increase=true;

    }
    else if (mood=='p'||mood=='P'){
        emotion[0]+=( emotion[0]*alpha+ (fabsf(1-alpha))*(action*(1-mood_int)+inclination) ); 
        increase=false;
    }
    else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con anger nel calcolo del valore di emotion[0]");
    }
    

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[0]<=0){
        emotion[0]=0;
    }else if (emotion[0]>=1){
        emotion[0]=1;
    }
}



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
    servo4.decel = 5;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, annoyance_seq1, (sizeof(annoyance_seq1)/sizeof(annoyance_seq1[0])));
    run_servo_sequence(&servo2, annoyance_seq2, (sizeof(annoyance_seq2)/sizeof(annoyance_seq2[0])));
    run_servo_sequence(&servo3, annoyance_seq3, (sizeof(annoyance_seq3)/sizeof(annoyance_seq3[0])));
    run_servo_sequence(&servo4, annoyance_seq4, (sizeof(annoyance_seq4)/sizeof(annoyance_seq4[0])));

    //mood update
    mood_manager(mood,increase,0.1);
    
    
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
    printf("Annoyance animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void anger(void){
    printf("\033[35mAnger animation started!\033[0m\n");
        if (emotion_running) {
        printf("an emotion is already running — skipping\n");
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

    //mood update
    mood_manager(mood,increase,0.2);
   

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
    printf("Anger animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void rage(void){
    printf("\033[35mRage animation started!\033[0m\n");
        if (emotion_running) {
        printf("an emotion is already running — skipping\n");
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

    servo4.target_speed = 100;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 10;
    servo4.running = true;

    

    run_servo_sequence(&servo1, rage_seq1, (sizeof(rage_seq1)/sizeof(rage_seq1[0])));
    run_servo_sequence(&servo2, rage_seq2, (sizeof(rage_seq2)/sizeof(rage_seq2[0])));
    run_servo_sequence(&servo3, rage_seq3, (sizeof(rage_seq3)/sizeof(rage_seq3[0])));
    run_servo_sequence(&servo4, rage_seq4, (sizeof(rage_seq4)/sizeof(rage_seq4[0])));

    //mood update
    mood_manager(mood,increase,0.5);
    
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
    printf("Rage animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);

}

#include "trust.h"
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

static const char *TAG = "trust";
static char previous_action[16];
static float beta=0;
static float inclination;
static float alpha;

esp_err_t trust_init(void)
{
    ESP_LOGI(TAG, "trust init");
    // inizializzazioni hardware / driver qui

    //calcolare alfa in base a J o P    circa forgetfulness (People with the Judging (J) personality trait feel most comfortable when the course ahead is well-marked. They would rather come up with five backup plans than deal with events as they come.Prospecting (P) individuals are much more flexible when it comes to dealing with unexpected challenges. )
    if(personality[3]=='J'){
        alpha=0.2;
    }else if (personality[3]=='P'){
        alpha=0.3;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con trust nel calcolo di alpha");
    }    
    //calcolare inclination in base alla personality  S o N (Intuitive personality types rely on imagining the past and future potential of what they see. Those with the Observant style are more interested in observable facts and more straightforward outcomes. They prefer to avoid layering too much interpretation on what they see.)
    if(personality[1]=='S'){
        inclination=0.001;
    }else if (personality[1]=='N'){
        inclination=0.005;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con trust nel calcolo di inclination");
    }    
    //printf("\nTrust parameters are initialized\n");
    return ESP_OK;
}

void trust_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        //increase gamma
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        //gamma resets
        beta=0;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con trust nel calcolo di gamma");
    }
    if(strcmp("hug",action_name)==0||strcmp("caress",action_name)==0||strcmp("hit",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[7]+=( emotion[7]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+beta*inclination)); 
    }
    else if (mood=='p'||mood=='P'){
/*      printf("emotion[1] at t-1: %.3f\n",emotion[7]);
        printf("alpha: %.3f\n", alpha);
        printf("mood: %.3f\n",mood_int);
        printf("emotion[1]*alpha : %.3f\n", emotion[7]*alpha);
        printf("fabs(1-%.3f): %.3f\n",alpha, fabs(1-alpha));
        printf("action*mood= %.3f*%.3f : %.3f\n",action,mood_int,action*mood_int);
        printf("beta*inclination= %.3f*%.3f: %.3f\n",beta, inclination, beta*inclination);
        printf("previous_action: %s\n", previous_action);
*/        emotion[7]+=( emotion[7]*alpha+ fabsf((1-alpha))*(action*(1-mood_int)+beta*inclination) );

    }
    else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con trust nel calcolo del valore di emotion[1]");

    }
//    printf("emotion[1] at t: %.3f\n",emotion[1]);
    if (emotion[7]<=0){
        emotion[7]=0;
    }else if (emotion[7]>=1){
        emotion[7]=1;
    }
}

void acceptance(void){
    printf("\033[35mAcceptance triggered\033[0m\n");
}
void trust(void){
    printf("\033[35mTrust triggered\033[0m\n");
}
void admiration(void){
    printf("\033[35mAdmiration triggered!\033[0m\n");
    if (emotion_running) {
        printf("an emotion is already running — skipping\n");
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

    servo3.target_speed = 10;
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 10;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 5;
    servo4.running = true;

    

    run_servo_sequence(&servo1, admiration_seq1, (sizeof(admiration_seq1)/sizeof(admiration_seq1[0])));
    run_servo_sequence(&servo2, admiration_seq2, (sizeof(admiration_seq2)/sizeof(admiration_seq2[0])));
    run_servo_sequence(&servo3, admiration_seq3, (sizeof(admiration_seq3)/sizeof(admiration_seq3[0])));
    run_servo_sequence(&servo4, admiration_seq4, (sizeof(admiration_seq4)/sizeof(admiration_seq4[0])));

    
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
    printf("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
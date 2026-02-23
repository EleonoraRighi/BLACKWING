#include "joy.h"
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


static const char *TAG = "joy";
static char previous_action[16];
static float beta=0;
static float inclination;
static float alpha;

esp_err_t joy_init(void)
{
    ESP_LOGI(TAG, "joy init");
    // inizializzazioni hardware / driver qui

    //calcolare alfa in base a J o P    circa forgetfulness (People with the Judging (J) personality trait feel most comfortable when the course ahead is well-marked. They would rather come up with five backup plans than deal with events as they come.Prospecting (P) individuals are much more flexible when it comes to dealing with unexpected challenges. )
    if(personality[3]=='J'){
        alpha=0.2;
    }else if (personality[3]=='P'){
        alpha=0.3;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con joy nel calcolo di alpha");
    }    
    //calcolare inclination in base alla personality  S o N (Intuitive personality types rely on imagining the past and future potential of what they see. Those with the Observant style are more interested in observable facts and more straightforward outcomes. They prefer to avoid layering too much interpretation on what they see.)
    if(personality[1]=='S'){
        inclination=0.001;
    }else if (personality[1]=='N'){
        inclination=0.005;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con joy nel calcolo di inclination");
    }    
    //printf("\nJoy parameters are initialized\n");
    return ESP_OK;
}

void joy_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        //increase gamma
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        //gamma resets
        beta=0;
    } else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con joy nel calcolo di gamma");
    }
        if(strcmp("hit",action_name)==0||strcmp("noise",action_name)==0||strcmp("closeness",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
/*        printf("emotion[3] at t-1: %.3f\n",emotion[3]);
        printf("alpha: %.3f\n", alpha);
        printf("mood: %.3f\n",mood_int);
        printf("emotion[3]*alpha : %.3f\n", emotion[3]*alpha);
        printf("fabs(1-%.3f): %.3f\n",alpha, fabs(1-alpha));
        printf("action*mood= %.3f*%.3f : %.3f\n",action,mood_int,action*mood_int);
        printf("beta*inclination= %.3f*%.3f: %.3f\n",beta, inclination, beta*inclination);
        printf("previous_action: %s\n", previous_action);
        printf("(1-mood)= %.2f\n", (1-mood_int));

        printf("emotion[3]+=(emotion[3]*alpha+ ((fabsf(1-alpha))*(action*(1-mood_int)+beta*inclination)) )\n %.3f+=(%.3f+(%.3f*(%.3f+%.3f)))",emotion[3], emotion[3]*alpha, fabs(1-alpha), action*(1-mood_int), beta*inclination);

*/        emotion[3]+=(emotion[3]*alpha+ ((fabsf(1-alpha))*(action*(1-mood_int)+beta*inclination)) );
        
//        printf("emotion[3]=%.3f",emotion[3]);
    }
    else if (mood=='p'||mood=='P'){
/*      printf("emotion[3] at t-1: %.3f\n",emotion[3]);
        printf("alpha: %.3f\n", alpha);
        printf("mood: %.3f\n",mood_int);
        printf("emotion[3]*alpha : %.3f\n", emotion[3]*alpha);
        printf("fabs(1-%.3f): %.3f\n",alpha, fabs(1-alpha));
        printf("action*mood= %.3f*%.3f : %.3f\n",action,mood_int,action*mood_int);
        printf("beta*inclination= %.3f*%.3f: %.3f\n",beta, inclination, beta*inclination);
        printf("previous_action: %s\n", previous_action);
*/        emotion[3]+=( emotion[3]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+beta*inclination));

    }
    else {
        ESP_LOGI(TAG, "Qualcosa è andato storto con joy nel calcolo del valore di emotion[3]");

    }
//    printf("emotion[3] at t: %.3f\n",emotion[3]);
    if (emotion[3]<=0){
        emotion[3]=0;
    }else if (emotion[3]>=1){
        emotion[3]=1;
    }
}

void serenity(void){
    printf("\033[35mSerenity triggered\033[0m\n");
    if (emotion_running) {
        printf("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;

    servo1.target_speed = 15;
    servo1.accel = 10;
    servo1.decel = 10;
    servo1.min_speed = 1;
    servo1.running = true;

    servo2.target_speed = 10;
    servo2.accel = 10;
    servo2.decel = 10;
    servo2.min_speed = 1;
    servo2.running = true;

    servo3.target_speed = 200;
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 10;
    servo3.running = true;

    servo4.target_speed = 200;
    servo4.accel = 50;
    servo4.decel = 50;
    servo4.min_speed = 50;
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
    printf("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void joy(void){
    printf("\033[35mJoy triggered\033[0m\n");
    if (emotion_running) {
        printf("an emotion is already running — skipping\n");
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
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 50;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 10;
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
    printf("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void ecstasy(void){
    printf("\033[35mEcstasy triggered\033[0m\n");
    if (emotion_running) {
        printf("an emotion is already running — skipping\n");
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
    servo3.accel = 10;
    servo3.decel = 10;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 80;
    servo4.accel = 10;
    servo4.decel = 10;
    servo4.min_speed = 10;
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
    printf("Apprehension animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
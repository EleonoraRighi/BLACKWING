#include "surprise.h"
#include "main.h"
#include "curiosity.h"
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



static const char *TAG = "surprise";
static char previous_action[16]="";
//static float beta=0;
//static float inclination;
//static float alpha;

SemaphoreHandle_t surprise_mutex;
bool surprise_active= false;
float JP_offset;
float IE_scaler;


// -----------------------------
// Task che scala la sorpresa (valori discreti)
// -----------------------------
void surprise_scaler_task(void *pvParameter) {

    float surprise_durata = 10.0f;  
    float surprise_temp=0;
    
    if (personality[2]=='J'){  
        surprise_temp = 0.1f;         
    } else if (personality[2]=='P'){
        surprise_temp = 0.3f;    
    }

    float surprise_passo = surprise_temp;
    float surprise_decremento = 1.0f / (surprise_durata / surprise_passo);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(surprise_passo * 1000));

        if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {

            if (surprise_active && emotion[5] > 0.0f) {
                emotion[5] -= surprise_decremento;
                if (emotion[5] <= 0.0f) {
                    emotion[5] = 0.0f;
                    surprise_active = false; // FINE sorpresa
                }
            }

            xSemaphoreGive(surprise_mutex);
        }
        vTaskDelay(1); //lascia respirare il watchdog
    }
    
}



esp_err_t surprise_init(void)
{
    ESP_LOGI(TAG, "surprise init");
    // inizializzazioni hardware / driver qui
    surprise_mutex = xSemaphoreCreateMutex();
    if (personality[2]=='J'){  //people with J personality don't do well with unexpected events
        JP_offset = 0.2;           
    }else if (personality[2]=='P'){
        JP_offset = 0;    
    }
    if (personality[0]=='E'){  //people with J personality don't do well with unexpected events
        JP_offset = 1;           
    }else if (personality[0]=='I'){
        JP_offset = 0.5;    
    }

    xTaskCreate(surprise_scaler_task, "scaler_task", 3072, NULL, 5, NULL);

   //printf("\nSurprise parameters are initialized\n");
    return ESP_OK;
}

void surprise_modify(float action, const char *action_name){

    
    //sorprese di livello 1 (ok, strano... non me lo aspettavo):  "qualunque cosa che non sia caress o hug", "hug"---->DONE                                                               
    //                                                              + "qualsiasi cosa tranne hug e caress o hit", "hit"---->DONE
    //sorpresa di livello 2 (Che confusione): "hug" o "caress","hit"--->DONE
    //                                           + viceversa--->DONE
    //                                           + "hit", "caress"---->DONE
    //sorpresa di livello 3 (Jumpscare): "qualsiasi cosa che non sia noise", "noise"--->DONE
    //                                      + "qualsiasi cosa che non sia bug", "bug" --->DONE   
    
    //printf("\nprevious action:\t %s\ncurrent action:\t %s\n", previous_action,action_name);

    if (strcmp(action_name, "hug")==0){
        if((strcmp(previous_action,"hug")!=0 && strcmp(previous_action, "caress")!=0)  ){
            if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                emotion[5]=0.5 + JP_offset;
                surprise_active = true;
                curiosity+=0.5*IE_scaler;
                //printf("Sorpresa di livello 1: ricevuto un 'hug' senza preavviso! (Una bella sorpresa)");
                xSemaphoreGive(surprise_mutex);
            }
        }else if ((strcmp(previous_action, "hit")==0)    ){
            if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                emotion[5]=0.75 + JP_offset;
                surprise_active = true;
                curiosity+=0.5*IE_scaler;
                //printf("Sorpresa di livello 2: ricevuto un 'hug' dopo un 'hit'! (Che confusione...)");
                xSemaphoreGive(surprise_mutex);            
            }

        }else if((strcmp(previous_action,"hug")==0 || strcmp(previous_action, "caress")==0)  ){
            if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                emotion[5]=0 + JP_offset;
                surprise_active = true;
                xSemaphoreGive(surprise_mutex);
            }
        }
    }else if (strcmp(action_name, "bug")==0){
        if (strcmp(previous_action,"bug")!=0){
                if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                    emotion[5]=0.9 + JP_offset;
                    surprise_active = true;
                    curiosity+=0.5*IE_scaler;
                    //printf("Sorpresa di livello 3: ricevuto un 'bug'! (Jumpscare!)");
                    xSemaphoreGive(surprise_mutex);
                }
        }else if (strcmp(previous_action,"bug")==0){
                if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                    emotion[5]=0 + JP_offset;
                    surprise_active = true;
                    xSemaphoreGive(surprise_mutex);
                }
        }

    }else if(strcmp(action_name,"noise")==0){
        if (strcmp(previous_action,"noise")!=0){
                if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                    emotion[5]=0.9 + JP_offset;
                    surprise_active = true;
                    curiosity+=0.5*IE_scaler;
                    printf("Sorpresa di livello 3: ricevuto un 'noise'! (Jumpscare!)");
                    xSemaphoreGive(surprise_mutex);
                }
        }        
    }else if(strcmp(action_name,"hit")==0){
        if((strcmp(previous_action,"caress")==0 || strcmp(previous_action,"hug")==0)    ){
                if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                emotion[5]=0.75 + JP_offset;
                surprise_active = true;
                curiosity+=0.5*IE_scaler;
                printf("Sorpresa di livello 2: ricevuto un 'hit' dopo un 'hug' o un 'caress'! (Che confusione...)");
                xSemaphoreGive(surprise_mutex);
                }
        }else if((strcmp(previous_action, "hit")!=0)     ){
            if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                emotion[5]=0.5 + JP_offset;
                surprise_active = true;
                curiosity+=0.5*IE_scaler;
                printf( "Sorpresa di livello 1: ricevuto un 'hit'! (Non me lo aspettavo)");
                xSemaphoreGive(surprise_mutex);
                }
        }
        
    }else if(strcmp(action_name,"caress")==0){
        if((strcmp(previous_action,"hit")==0)    ){
            if (xSemaphoreTake(surprise_mutex, portMAX_DELAY)) {
                emotion[5]=0.75 + JP_offset;
                surprise_active = true;
                curiosity+=0.5*IE_scaler;
                printf("Sorpresa di livello 2: ricevuto un 'caress' dopo un 'hit'! (Che confusione...)");
                xSemaphoreGive(surprise_mutex);
            }
        }
    }
    
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    if (emotion[5]<=0){
        emotion[5]=0;
    }else if (emotion[5]>=1){
        emotion[5]=1;
    }
    //    printf("emotion[5] at t: %.3f\n",emotion[5]);
}


void distraction(void){
    printf("\033[35mDistraction function triggered\033[0m\n");

    if (emotion_running) {
        printf("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


    servo1.target_speed = 2000;
    servo1.accel = 50;
    servo1.decel = 50;
    servo1.min_speed = 800;
    servo1.running = true;

    servo2.target_speed = 600;
    servo2.accel = 5;
    servo2.decel = 5;
    servo2.min_speed = 10;
    servo2.running = true;

    servo3.target_speed = 500;
    servo3.accel = 120;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 300;
    servo4.accel = 50;
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
    servo1.min_speed = 50;
    servo1.running = true;

    printf("Distraction animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
    
}
void surprise(void){    
    printf("\033[35mSurprise function triggered\033[0m\n");
        if (emotion_running) {
        printf("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


    servo1.target_speed = 1000;
    servo1.accel = 50;
    servo1.decel = 50;
    servo1.min_speed = 800;
    servo1.running = true;

    servo2.target_speed = 300;
    servo2.accel = 50;
    servo2.decel = 50;
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

    printf("Distraction animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
void amazement(void){
    printf("\033[35mAmazement function triggered\033[0m\n");
            if (emotion_running) {
        printf("an emotion is already running — skipping\n");
        return;
    }
    emotion_running = true;


    servo1.target_speed = 1000;
    servo1.accel = 1000;
    servo1.decel = 1000;
    servo1.min_speed = 800;
    servo1.running = true;

    servo2.target_speed = 400;
    servo2.accel = 50;
    servo2.decel = 50;
    servo2.min_speed = 100;
    servo2.running = true;

    servo3.target_speed = 100;
    servo3.accel = 120;
    servo3.decel = 50;
    servo3.min_speed = 100;
    servo3.running = true;

    servo4.target_speed = 300;
    servo4.accel = 50;
    servo4.decel = 100;
    servo4.min_speed = 100;
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

    printf("Distraction animation ended, returning to default position\n");
    run_servo_sequence(&servo1, default_seq1, 1);
    run_servo_sequence(&servo2, default_seq2, 1);
    run_servo_sequence(&servo3, default_seq3, 1);
    run_servo_sequence(&servo4, default_seq4, 1);
}
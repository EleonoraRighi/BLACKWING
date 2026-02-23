#include "idle.h"
#include "thresholds.h"
#include "main.h"
#include "curiosity.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "math.h"
#include "thresholds.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "esp_random.h"
#include <stdio.h>
#include <string.h>

#define TIMEOUT_MS 5000   // timeout = 5 secondi
static const char *TAG = "idle";
static float IE_scaler; //scaler based on I or E personality type

esp_err_t idle_init(void){
    //Based on I or E (E is more interested in exploring its sorroundings when idle)
    if (personality[0]=='I'){
        IE_scaler=1.0f;
    }else if (personality[0]=='E'){ 
        IE_scaler=0.5f;
    }
    //ESP_LOGI(TAG, "Idle component initialized.");
    return ESP_OK;
}

void idle(void){
    if(time_since_last_uart < TIMEOUT_MS){
        ESP_LOGD(TAG, "Idle function skipped due to recent UART activity.");
        return;
    }

    if (curiosity>0 && curiosity<THRESHOLD_1*IE_scaler){
        //not moving
        printf("Idle: resting\n");
    }else if(curiosity>=THRESHOLD_1*IE_scaler && curiosity<THRESHOLD_2*IE_scaler){
        //look around slowly
        printf("Idle: looking around slowly\n");
    }else if(curiosity>=THRESHOLD_2*IE_scaler && curiosity<THRESHOLD_3*IE_scaler){
        //look around and move a bit
        printf("Idle: looking around and moving a bit\n");
    }else if(curiosity>=THRESHOLD_3*IE_scaler && curiosity<1){
        //move a lot
        printf("Idle: moving a lot\n");  
        printf("THRESHOLD 3= %.2f*%.2f!\n", THRESHOLD_3,IE_scaler); 
    }
    vTaskDelay(pdMS_TO_TICKS(100)); // delay to allow terminal to sample
    ESP_LOGD(TAG, "Idle function called.");
}
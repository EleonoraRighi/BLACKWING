#include "curiosity.h"
#include "main.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "math.h"
#include "thresholds.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "stdbool.h"


static const char *TAG = "curiosity";
float curiosity=0.0f;
float curiosity_passo;
SemaphoreHandle_t curiosity_mutex;

// -----------------------------
// Task che scala gradualmente la curiosity
// -----------------------------
void curiosity_scaler_task(void *pvParameter) {
    const float curiosity_durata = 300.0f;  // secondi
    const float curiosity_decremento = 1.0f / (curiosity_durata / curiosity_passo);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(curiosity_passo * 1000));

        if (xSemaphoreTake(curiosity_mutex, portMAX_DELAY)) {
            if (curiosity > 0.0f) {
                curiosity -= curiosity_decremento;
                if (curiosity < 0.0f) mood_int = 0.0f;
            }
            xSemaphoreGive(curiosity_mutex);
        }
        vTaskDelay(10);// lascia respirare il watchdog
    }
}

void curiosity_increase(float value){
    if (xSemaphoreTake(curiosity_mutex, portMAX_DELAY)) {
        curiosity += value;
        if (curiosity > 1.0f) curiosity = 1.0f;
        xSemaphoreGive(curiosity_mutex);
    }
}
void curiosity_decrease(float value){
    if (xSemaphoreTake(curiosity_mutex, portMAX_DELAY)) {
        curiosity -= value;
        if (curiosity < 0.0f) curiosity = 0.0f;
        xSemaphoreGive(curiosity_mutex);
    }
}
esp_err_t curiosity_init(void)
{
    ESP_LOGI(TAG, "curiosity init");
    // inizializzazioni hardware / driver qui

    if(personality[0]=='E'){
        curiosity_passo=0.3f;
    }else if(personality[0]=='I'){
        curiosity_passo=0.1f;
    }

    curiosity_mutex = xSemaphoreCreateMutex();
    xTaskCreate(curiosity_scaler_task, "scaler_task", 3072, NULL, 5, NULL);

   //printf("\nCuriosity parameters are initialized\n");
    return ESP_OK;
}
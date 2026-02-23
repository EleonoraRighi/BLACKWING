#include <stdio.h>
#include <string.h>
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/semphr.h"
#include "driver/uart.h"

#include "adxl345.h"
#include "motor_declarations.h"

#include "anger.h"
#include "anticipation.h"
#include "disgust.h"
#include "fear.h"
#include "idle.h"
#include "joy.h"
#include "sadness.h"
#include "surprise.h"
#include "trust.h"




#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024
#define TAG "ESP"

const char *emotion_name[]={"Anger", "Disgust","Fear","Joy","Sadness","Surprise","Anticipation","Trust"};
float emotion[8]={0.1,0.1,0.1,0.1,0.1,0.0, 0.0, 0.0};  // Anger, Disgust, Fear, Joy, Sadness, Surprise, Anticipation, Trust

int emotion_th[8]={0,0,0,0,0,0,0,0};

int i=0; //for cycles and more


//Function to send messages to UART
void uart_send_msg(const char *msg) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "[ESP] %s\n", msg);
        uart_write_bytes(UART_PORT, buffer, strlen(buffer));
}

static void idle1_task(void *pvParameter) {
    (void)pvParameter;
    idle_1();
    vTaskDelete(NULL);
}

static void idle2_task(void *pvParameter) {
    (void)pvParameter;
    idle_2();
    vTaskDelete(NULL);
}

static bool is_idle_command(const char *cmd) {
    return (strcmp(cmd, "idle1") == 0) ||
           (strcmp(cmd, "idle2") == 0) ||
           (strcmp(cmd, "idle3") == 0);
}

static bool any_servo_busy(void) {
    return servo1.busy || servo2.busy || servo3.busy || servo4.busy;
}

static bool is_preempt_command(const char *cmd) {
    return (strcmp(cmd, "annoyance") == 0) ||
           (strcmp(cmd, "anger") == 0) ||
           (strcmp(cmd, "rage") == 0) ||
           (strcmp(cmd, "interest") == 0) ||
           (strcmp(cmd, "anticipation") == 0) ||
           (strcmp(cmd, "vigilance") == 0) ||
           (strcmp(cmd, "boredom") == 0) ||
           (strcmp(cmd, "disgust") == 0) ||
           (strcmp(cmd, "loathing") == 0) ||
           (strcmp(cmd, "apprehension") == 0) ||
           (strcmp(cmd, "fear") == 0) ||
           (strcmp(cmd, "terror") == 0) ||
           (strcmp(cmd, "serenity") == 0) ||
           (strcmp(cmd, "joy") == 0) ||
           (strcmp(cmd, "ecstasy") == 0) ||
           (strcmp(cmd, "pensiveness") == 0) ||
           (strcmp(cmd, "sadness") == 0) ||
           (strcmp(cmd, "grief") == 0) ||
           (strcmp(cmd, "distraction") == 0) ||
           (strcmp(cmd, "surprise") == 0) ||
           (strcmp(cmd, "amazement") == 0) ||
           (strcmp(cmd, "acceptance") == 0) ||
           (strcmp(cmd, "trust") == 0) ||
           (strcmp(cmd, "admiration") == 0) ||
           (strcmp(cmd, "bow") == 0);
}

static void abort_idle_if_needed(const char *cmd) {
    if (strcmp(cmd, "START") == 0 || is_idle_command(cmd) || !is_preempt_command(cmd)) {
        return;
    }

    if (!idle_running) {
        return;
    }

    idle_abort_requested = true;

    TickType_t start = xTaskGetTickCount();
    const TickType_t timeout = pdMS_TO_TICKS(2000);
    while ((xTaskGetTickCount() - start) < timeout) {
        if (!idle_running && !emotion_running && !any_servo_busy()) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Task per gestire i comandi da UART (TEMPORANEO)
void uart_task(void *pvParameter) {
    uint8_t data[BUF_SIZE];

     while (1) {
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 2, pdMS_TO_TICKS(1000));
        if (len > 0) {
            data[len] = '\0'; // termina la stringa
            // Rimuove eventuali \r\n
            for (int i = 0; i < len; i++) {
                if (data[i] == '\r' || data[i] == '\n') {
                    data[i] = '\0';
                    break;
                }
            }
            abort_idle_if_needed((char *)data);

 /*            char msg[128];
            snprintf(msg, sizeof(msg), "Input received by the ESP: %s\n", data);
            uart_send_msg(msg);
*/    
 
             // --- Confronta i comandi testuali ---
            if (strcmp((char *)data, "START") == 0) {
                servo_init_all();  // initialize motor declarations
                servo_manager_init(); // initialize servo manager

                vTaskDelay(pdMS_TO_TICKS(1000));

                uart_send_msg("Esp is ready!\n");
                
            //possible inputs from Jetson
            }else if (strcmp((char *)data, "annoyance") == 0) {
                    annoyance();
                    //uart_send_msg("Input received by the ESP: annoyance\n");
            }else if (strcmp((char *)data, "anger") == 0) {
                    anger(); 
                    //uart_send_msg("Input received by the ESP: anger\n");       
            }else if (strcmp((char *)data, "rage") == 0) {
                    rage();
                    //uart_send_msg("Input received by the ESP: rage\n");
            }else if (strcmp((char *)data, "interest") == 0) {
                    interest();
                    //uart_send_msg("Input received by the ESP: interest\n");
            }else if (strcmp((char *)data, "anticipation") == 0) {
                    anticipation();
                    //uart_send_msg("Input received by the ESP: anticipation\n");
            }else if (strcmp((char *)data, "vigilance") == 0) {
                    vigilance();
                    //uart_send_msg("Input received by the ESP: vigilance\n");
            }else if (strcmp((char *)data, "boredom") == 0) {
                    boredom();
                    uart_send_msg("Input received by the ESP: boredom\n");
            }else if (strcmp((char *)data, "disgust") == 0) {
                    disgust();
                    //uart_send_msg("Input received by the ESP: disgust\n");
            }else if (strcmp((char *)data, "loathing") == 0) {
                    loathing();
                    //uart_send_msg("Input received by the ESP: loathing\n");
            }else if (strcmp((char *)data, "apprehension") == 0) {
                    apprehension();
                    uart_send_msg("Input received by the ESP: apprehension\n");
            }else if (strcmp((char *)data, "fear") == 0) {
                    fear();
                    //uart_send_msg("Input received by the ESP: fear\n");
            }else if (strcmp((char *)data, "terror") == 0) {
                    terror();
                    //uart_send_msg("Input received by the ESP: terror\n");
            }else if (strcmp((char *)data, "serenity") == 0) {
                    serenity();
                    //uart_send_msg("Input received by the ESP: serenity\n");
            }else if (strcmp((char *)data, "joy") == 0) {
                    joy();
                    //uart_send_msg("Input received by the ESP: joy\n");
            }else if (strcmp((char *)data, "ecstasy") == 0) {
                    ecstasy();
                    //uart_send_msg("Input received by the ESP: ecstasy\n");
            }else if (strcmp((char *)data, "pensiveness") == 0) {
                    pensiveness();
                    //uart_send_msg("Input received by the ESP: pensiveness\n");
            }else if (strcmp((char *)data, "sadness") == 0) {
                    sadness();
                    //uart_send_msg("Input received by the ESP: sadness\n");
            }else if (strcmp((char *)data, "grief") == 0) {
                    grief();
                    //uart_send_msg("Input received by the ESP: grief\n");
            }else if (strcmp((char *)data, "distraction") == 0) {
                    distraction();
                    //uart_send_msg("Input received by the ESP: distraction\n");
            }else if (strcmp((char *)data, "surprise") == 0) {
                    surprise();
                    //uart_send_msg("Input received by the ESP: surprise\n");
            }else if (strcmp((char *)data, "amazement") == 0) {
                    amazement(); 
                    //uart_send_msg("Input received by the ESP: amazement\n");
            }else if (strcmp((char *)data, "acceptance") == 0) {
                    acceptance(); 
                    //uart_send_msg("Input received by the ESP: acceptance\n");
            }else if (strcmp((char *)data, "trust") == 0) {
                    trust(); 
                    //uart_send_msg("Input received by the ESP: trust\n");
            }else if (strcmp((char *)data, "admiration") == 0) {
                    admiration();          
                    //uart_send_msg("Input received by the ESP: admiration\n"); 
            }else if (strcmp((char *)data, "idle1") == 0) {
                    xTaskCreate(idle1_task, "idle1_task", 4096, NULL, 5, NULL);
                    //uart_send_msg("Input received by the ESP: idle_1\n");         
            }else if (strcmp((char *)data, "idle2") == 0) {
                    xTaskCreate(idle2_task, "idle2_task", 4096, NULL, 5, NULL);
                    //uart_send_msg("Input received by the ESP: idle_2\n"); 
            }else if (strcmp((char *)data, "idle3") == 0) {
                    idle_3();          
                    //uart_send_msg("Input received by the ESP: idle_3\n"); 
            }else if (strcmp((char *)data, "bow") == 0) {
                    bow();          
                    //uart_send_msg("Input received by the ESP: bow\n"); 
            } else {
                uart_send_msg("Input received by the ESP: unknown command\n");


            }
            
        }

        
    }
}

void app_main(void)
{           

   // UART configuration
    uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT, &uart_cfg);
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);
        

    // Crea i task paralleli
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);

    adxl345_start();

    // Loop principale di debug
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
      
}

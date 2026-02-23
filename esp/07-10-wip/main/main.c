#include <stdio.h>
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/semphr.h"
#include "driver/uart.h"

#include "idle.h"
#include "thresholds.h"
#include "thresholds_control.h" 
#include "motor_declarations.h"
#include "joy.h"
#include "sadness.h"
#include "fear.h"
#include "anger.h"
#include "surprise.h"
#include "disgust.h"
#include "curiosity.h"
#include "trust.h"
#include "anticipation.h"


//task parallelo: MOOD con "mutex" per accesso safe alla variabile. 
    //positivo o negativo + intensità
    //decresce in base al tempo
    //aumenta o diminuisce in base alle emozioni ESPRESSE
    // velocità dipende da T o F (più veloce per F)
    // velocità influenzata da inattività per I o E

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024



static const char *TAG = "MoodScalerUART";
SemaphoreHandle_t mutex;
volatile uint64_t time_since_last_uart = 0;

char personality[16];
char mood_str[16];
char mood;
char mood_int_str[16];
float mood_int = 1.0f;
const char *emotion_name[]={"Anger", "Disgust","Fear","Joy","Sadness","Surprise","Anticipation","Trust"};
float emotion[8]={0.1,0.1,0.1,0.1,0.1,0.0, 0.0, 0.0};  // Anger, Disgust, Fear, Joy, Sadness, Surprise, Anticipation, Trust

int emotion_th[8]={0,0,0,0,0,0,0,0};

int i=0; //per i cicli for e altro


// Task che scala gradualmente il mood
void mood_scaler_task(void *pvParameter) {
    float mood_duration = MOOD_DEC_TIME;  // secondi
    float mood_step_temp=0;
   if (personality[2]=='F'){
        mood_step_temp = 0.2f;    // tempo step in secondi        
    }else if (personality[2]=='T'){
        mood_step_temp = 0.1f;    // tempo step in secondi
    }
    float mood_step = mood_step_temp;
    float mood_dec = 1.0f / (mood_duration / mood_step);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(mood_step * 1000));

        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            if (mood_int > 0.0f) {
                mood_int -= mood_dec;
                if (mood_int < 0.0f) mood_int = 0.0f;
            }
            xSemaphoreGive(mutex);
        }
    }
}

//Mood modifications manager
void mood_manager(char mood, bool inc, float value){

/*
    if (inc){
        mood_int+=value;
        printf("\nUpdated mood is increased to %.2f %s", mood_int, mood_str);
    }else{
        mood_int-=value;
        printf("\nUpdated mood is decreased to %.2f %s", mood_int, mood_str);
    }
*/
}

// Timer callback
void timer_callback(void *arg) {
    vTaskDelay(pdMS_TO_TICKS(10000)); // delay to express idle actions
    time_since_last_uart += 100; // timer ogni 100ms 
}

// Task per gestire i comandi da UART (TEMPORANEO)
void uart_task(void *pvParameter) {
    uint8_t data[BUF_SIZE];


     while (1) {
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 2, pdMS_TO_TICKS(1000));
        if (len > 0) {
            if (len >= BUF_SIZE - 1) len = BUF_SIZE - 2; // sicurezza
            data[len] = '\0';
            
            }
            
        if (len > 0) {
            data[len] = '\0'; // termina la stringa
            // Rimuove eventuali \r\n
            for (int i = 0; i < len; i++) {
                if (data[i] == '\r' || data[i] == '\n') {
                    data[i] = '\0';
                    break;
                }
            }
            time_since_last_uart = 0;  // azzera timer
            if (personality[0]=='I')
                curiosity-=0.05;
            else if(personality[0]=='E')
                curiosity+=0.1;
            
            ESP_LOGI(TAG, "Comando ricevuto: '%s'", (char *)data);

             // --- Confronta i comandi testuali ---
            if (strcmp((char *)data, "hit") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(0.3, "hit");
                    disgust_modify(0.08, "hit");
                    fear_modify(0.3, "hit");
                    joy_modify(-0.3, "hit");
                    sadness_modify(0.1, "hit");
                    surprise_modify(0.2, "hit");
                    anticipation_modify(-0.1, "hit");
                    trust_modify(-0.2, "hit");

                    ESP_LOGI(TAG, "Ricevuto un 'hit'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();

                  
                    
                 }
            } else if (strcmp((char *)data, "hug") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(-0.08, "hug");
                    disgust_modify(0, "hug");
                    fear_modify(-0.3, "hug");
                    joy_modify(0.25, "hug");
                    sadness_modify(-0.3, "hug");
                    surprise_modify(-0.1, "hug");
                    anticipation_modify(0.2, "hug");
                    trust_modify(0.3, "hug");

                    ESP_LOGI(TAG, "Ricevuto un 'hug'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();

                  
                    
                 }

            } else if (strcmp((char *)data, "caress") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(-0.1, "caress");
                    disgust_modify(0, "caress");
                    fear_modify(-0.15, "caress");
                    joy_modify(0.2, "caress");
                    sadness_modify(-0.15, "caress");
                    surprise_modify(-0.1, "caress");
                    anticipation_modify(0.1, "caress");
                    trust_modify(0.2, "caress");

                    ESP_LOGI(TAG, "Ricevuto un 'caress'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();

                  
                    
                 }

            } else if (strcmp((char *)data, "bug") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(0.15, "bug");
                    disgust_modify(0.4, "bug");
                    fear_modify(0.3, "bug");
                    joy_modify(-0.3, "bug");
                    sadness_modify(-0.1, "bug");
                    surprise_modify(0.3, "bug");
                    anticipation_modify(0.2, "bug");
                    trust_modify(-0.2, "bug");


                    ESP_LOGI(TAG, "Ricevuto un 'bug'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();

                  
                    
                 }

            } else if (strcmp((char *)data, "sorrow") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(-0.2, "sorrow");
                    disgust_modify(0.08, "sorrow");
                    fear_modify(-0.3, "sorrow");
                    joy_modify(-0.3, "sorrow");
                    sadness_modify(0.3, "sorrow");
                    surprise_modify(-0.2, "sorrow");
                    anticipation_modify(-0.1, "sorrow");
                    trust_modify(-0.2, "sorrow");

                    ESP_LOGI(TAG, "Ricevuto un 'sorrow'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();

                  
                    
                 }

            } else if (strcmp((char *)data, "noise") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(0.1, "noise");
                    disgust_modify(0.1, "noise");
                    fear_modify(0.2, "noise");
                    joy_modify(-0.3, "noise");
                    sadness_modify(-0.1, "noise");
                    surprise_modify(0.2, "noise");
                    anticipation_modify(0.1, "noise");
                    trust_modify(-0.1, "noise");

                    ESP_LOGI(TAG, "Ricevuto un 'noise'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();

                  
                    
                 }

            } else if (strcmp((char *)data, "closeness") == 0) {
                if (xSemaphoreTake(mutex, portMAX_DELAY)) {
                    anger_modify(-0.1, "closeness");
                    disgust_modify(0.1, "closeness");
                    fear_modify(0.1, "closeness");
                    joy_modify(-0.1, "closeness");
                    sadness_modify(-0.3, "closeness");
                    surprise_modify(0.2, "closeness");
                    anticipation_modify(0.2, "closeness");
                    trust_modify(0.3, "closeness");

                    ESP_LOGI(TAG, "Ricevuto un 'closeness'");
                    xSemaphoreGive(mutex);

                   for(i=0;i<NUM_EMOTIONS;i++){
                        printf("\n%s:%.3f ", emotion_name[i], emotion[i]);
                        vTaskDelay(pdMS_TO_TICKS(10)); // lascia respirare il watchdog
                    }
                    printf("\n");
                    update_all_thresholds();
                }  
            //for debug and building purposes
            }else if (strcmp((char *)data, "ang1") == 0) {
                    annoyance();
            }else if (strcmp((char *)data, "ang2") == 0) {
                    anger();        
            }else if (strcmp((char *)data, "ang3") == 0) {
                    rage();
            }else if (strcmp((char *)data, "ant1") == 0) {
                    interest();
            }else if (strcmp((char *)data, "ant2") == 0) {
                    anticipation();
            }else if (strcmp((char *)data, "ant3") == 0) {
                    vigilance();
            }else if (strcmp((char *)data, "dis1") == 0) {
                    boredom();
            }else if (strcmp((char *)data, "dis2") == 0) {
                    disgust();
            }else if (strcmp((char *)data, "dis3") == 0) {
                    loathing();
            }else if (strcmp((char *)data, "fear1") == 0) {
                    apprehension();
            }else if (strcmp((char *)data, "fear2") == 0) {
                    fear();
            }else if (strcmp((char *)data, "fear3") == 0) {
                    terror();
            }else if (strcmp((char *)data, "joy1") == 0) {
                    serenity();
            }else if (strcmp((char *)data, "joy2") == 0) {
                    joy();
            }else if (strcmp((char *)data, "joy3") == 0) {
                    ecstasy();
            }else if (strcmp((char *)data, "sad1") == 0) {
                    pensiveness();
            }else if (strcmp((char *)data, "sad2") == 0) {
                    sadness();
            }else if (strcmp((char *)data, "sad3") == 0) {
                    grief();
            }else if (strcmp((char *)data, "sur1") == 0) {
                    distraction();
            }else if (strcmp((char *)data, "sur2") == 0) {
                    surprise();
            }else if (strcmp((char *)data, "sur3") == 0) {
                    amazement(); 
            }else if (strcmp((char *)data, "tr1") == 0) {
                    acceptance(); 
            }else if (strcmp((char *)data, "tr2") == 0) {
                    trust(); 
            }else if (strcmp((char *)data, "tr3") == 0) {
                    admiration();                    

            } else {
                ESP_LOGW(TAG, "Comando sconosciuto: '%s'", (char *)data);


            }
            
        }

        
    }
}

// Funzione per leggere una stringa da UART (max n-1 caratteri)
size_t prev_printed_len = 0;
const char prompt[64];

void uart_read_string(char *buffer, size_t n)
{
    size_t index = 0;
    uint8_t data;
    size_t prompt_len=strlen(prompt);

    // svuota eventuale buffer residuo
    while (uart_read_bytes(UART_PORT, &data, 1, 0) > 0);

while (index < n - 1) {
    int len = uart_read_bytes(UART_PORT, &data, 1, pdMS_TO_TICKS(100));
    if (len > 0) {

        if (data == '\r' || data == '\n') {
            const char crlf[] = "\r\n";
            uart_write_bytes(UART_PORT, crlf, 2);
            break;
        }
        if(data==',') data='.';
        // --- BACKSPACE ---
        if (data == 0x08 || data == 0x7F) {
            if (index > 0) {
                index--;
                buffer[index] = '\0';
            }
        }
        else {
            // --- CARATTERE NORMALE ---
            buffer[index++] = (char)data;
            buffer[index] = '\0';
        }

        // ----- RISCRIVI RIGA (funziona con idf.py monitor) -----
        {
            // torna all'inizio della riga
            const char cr = '\r';
            uart_write_bytes(UART_PORT, &cr, 1);

            // prompt
            uart_write_bytes(UART_PORT, prompt, prompt_len);

            // buffer
            if (index > 0)
                uart_write_bytes(UART_PORT, buffer, index);

            // cancella caratteri residui se la riga precedente era più lunga
            size_t curr_len = prompt_len + index;
            if (prev_printed_len > curr_len) {
                size_t extra = prev_printed_len - curr_len;
                for (size_t i = 0; i < extra; i++) {
                    uart_write_bytes(UART_PORT, " ", 1);
                }

                // torna all'inizio e riscrivi prompt+buffer
                uart_write_bytes(UART_PORT, &cr, 1);
                uart_write_bytes(UART_PORT, prompt, prompt_len);
                if (index > 0)
                    uart_write_bytes(UART_PORT, buffer, index);
            }

            prev_printed_len = prompt_len + index;
        }

    }

    vTaskDelay(pdMS_TO_TICKS(10));
}

buffer[index] = '\0';
}


bool ask_personality(void){
    char prompt[]="\n\033[36mHI! What is my personality type? (es. INTP, ENFP, ISTJ, ...)\033[0m\n";
    uart_write_bytes(UART_PORT, prompt, strlen(prompt));

    uart_read_string(personality, sizeof(personality));

    for( i=0;i<sizeof(personality);i++){
        if (personality[i] >= 'a' && personality[i] <= 'z') {
            personality[i] = personality[i] - 32;        
        }
    }

    //input control
    if (personality[0]=='I' || personality[0]=='E'){
        if (personality[1]=='S' || personality[1]=='N'){
            if (personality[2]=='T' || personality[2]=='F'){
                if (personality[3]=='J' || personality[3]=='P'){
                    return true;
                } else {
                    printf("\n\033[1;31mError: last letter must be either J or P.\033[0m\n");
                    return false;
                }
            } else {
                printf("\n\033[1;31mError: third letter must be either T or F.\033[0m\n");
                return false;
            }
        } else {
            printf("\n\033[1;31mError: second letter must be either S or N.\033[0m\n");
            return false;
        }
    } else {
        printf("\n\033[1;31mError: first letter must be either I or E.\033[0m\n");
        return false;
    }
}
bool ask_mood(void){
    char prompt[]="\n\033[36mWhat is my mood right now? (pos or neg)\033[0m\n";
    uart_write_bytes(UART_PORT, prompt, strlen(prompt));
    uart_read_string(mood_str, sizeof(mood_str));


    //input control
    if (mood_str[0]=='p' || mood_str[0]=='P'){
        strcpy(mood_str,"positive");
        return true;
    } 
    if (mood_str[0]=='n' || mood_str[0]=='N' ){
        strcpy(mood_str,"negative");
        return true;
    } else {
        printf("\n\033[1;31mError: Try answering by only choosing between pos or neg.\033[0m\n");
        return false;
                }

}
bool ask_mood_intensity(void){
    char prompt[]="\n\033[36mWhat is the intensity of the mood I'm feeling? (choose a number between 0 and 1, using up to 2 decimals)\033[0m\n";
    uart_write_bytes(UART_PORT, prompt, strlen(prompt));    
    uart_read_string(mood_int_str, sizeof(mood_int_str));

    mood_int=atof(mood_int_str);
    //input control
    if (mood_int>=0 && mood_int<=1){
        return true;
    } else {
        printf("\n\033[1;31mError: The only acceptable values are between 0 and 1. eg 0.5.\033[0m\n");
        return false;
                }

}


void emotion_initializer(void){
//Initialization of the emotion array with fixed values, scaled based on mood and personality trait 3
    //multiplicative factor related to T/F personality trait (F are more sentimental, so they will be more influenced by mood)
    float F_scaler=1;
    if (personality[2]=='F')
        F_scaler=1.5;
    
    //Adjust emotions based on mood
    if (mood=='n'||mood=='N'){
        emotion[0]+=0.1*F_scaler;
        emotion[3]-=0.1*F_scaler;
        emotion[4]+=0.1*F_scaler;
    }
    if (mood=='p' || mood=='P'){
        emotion[1]-=0.1*F_scaler;
        emotion[3]+=0.1*F_scaler;
        emotion[4]-=0.1*F_scaler;
        emotion[6]+=0.1*F_scaler;
        emotion[7]+=0.1*F_scaler;
    }
    //make sure all emotions are between 0 and 1
    for(i=0;i<NUM_EMOTIONS;i++){
        if(emotion[i]<=0){
            emotion[i]=0;
        }else if(emotion[i]>=1){
            emotion[i]=1;
        }
    }
    
    printf("\n\033[36mSince my personality trait #3 is %c and my mood is %s\033[0m\n", personality[2], mood_str);
    vTaskDelay(pdMS_TO_TICKS(1000));    //delay to allow terminal to sample

    //print emotions initial values
    for(i=0;i<NUM_EMOTIONS;i++){
        printf("\n\033[36m%s:%.3f\033[0m ", emotion_name[i], emotion[i]);
        vTaskDelay(pdMS_TO_TICKS(10)); // to avoid watchdog reset
    }
    printf("\n");
}

void app_main(void)
{      
    
    ESP_LOGI(TAG, "Avvio programma...");
    
//Program initialization based on UART input from user
   // UART configuration
    uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_cfg);

    // Create periodic software timer (100 ms)
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timer_callback,
        .name = "uart_timer"
    };

    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 100000); // 100ms in microseconds


    
    servo_init_all();  // initialize motor declarations
    servo_manager_init(); // initialize servo manager

ledc_set_duty(LEDC_LOW_SPEED_MODE, servo1.channel, 77);   // 1.5ms impulse on 50Hz
ledc_update_duty(LEDC_LOW_SPEED_MODE, servo1.channel);




//Initializing parameters asking user input
    // First Question
    bool verify_1=false;
    int counter=0;
    while(verify_1==false){
        verify_1=ask_personality();
        if (verify_1==false && counter>0){
            printf("\nSeems like there was an error in the personality type. Please try again.\n");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        counter++;
    }
    printf("\nYou chose: %s\n", personality);

    // Second Question
    bool verify_2=false;
    while(verify_2==false){
        verify_2=ask_mood();
        if (verify_2==false){
            printf("\nSeems like there was an error in the mood choice. Please try again.\n");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    mood=mood_str[0];
    printf("\nYou chose: %s mood\n", mood_str);

    // Third Question
    bool verify_3=false;
    while(verify_3==false){
        verify_3=ask_mood_intensity();
        if (verify_3==false){
            printf("\nSeems like there was an error in the mood intensity choice. Please try again.\n");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    printf("\nYou chose : %.0f%% %s mood\n", mood_int*100, mood_str);

    
    // Answer summary
    printf("\nAnswer Summary:\n");
    printf("Personality: %s\n", personality);
    printf("Mood: %s\n", mood_str);
    printf("Intensity of mood:%.2f\n", mood_int);

    vTaskDelay(pdMS_TO_TICKS(1000));          //delay to allow terminal to sample


    //Initialize emotions values
    emotion_initializer();

    //initialize components
    idle_init();
    curiosity_init();
    anger_init();
    anticipation_init();
    disgust_init();
    fear_init();
    joy_init();
    sadness_init();
    surprise_init();
    trust_init();
    vTaskDelay(pdMS_TO_TICKS(1000));

/*   // Avvia il test di movimento indipendente
    xTaskCreatePinnedToCore(
        (TaskFunction_t)anger_test,   // funzione da eseguire
        "anger_test",                 // nome del task
        4096,                         // stack size
        NULL,                         // parametro (non serve)
        2,                            // priorità
        NULL,                         // handle del task (non serve qui)
        1                             // core (1 o 0 a seconda del tuo ESP32)
    );
*/ 



    // Inizializza mutex
    mutex = xSemaphoreCreateMutex();

    // Configura UART (usa UART0 collegata via USB)
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT, &uart_config);
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Crea i task paralleli
    xTaskCreate(mood_scaler_task, "scaler_task", 3072, NULL, 5, NULL);
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);

/*
    // Loop principale di debug
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
        //    ESP_LOGI(TAG, "Valore attuale = %.3f", mood_int);
            xSemaphoreGive(mutex);
        }
    }
*/      
}


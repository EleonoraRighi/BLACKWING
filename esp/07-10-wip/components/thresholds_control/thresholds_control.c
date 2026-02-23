#include "thresholds_control.h"
#include "main.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "math.h"
#include "thresholds.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "driver/ledc.h"

// --- Include delle reazioni ---
#include "anger.h"
#include "fear.h"
#include "sadness.h"
#include "disgust.h"
#include "joy.h"
#include "surprise.h"
#include "trust.h"
#include "anticipation.h"

float t1;
float t2;
float t3;

// --- Reazioni definite altrove ---
void annoyance(); void anger(); void rage();
void boredom(); void disgust(); void loathing();
void apprehension(); void fear(); void terror();
void serenity(); void joy(); void ecstasy();
void pensiveness(); void sadness(); void grief();
void distraction(); void surprise(); void amazement();
void interest(); void anticipation(); void vigilance();
void acceptance(); void trust(); void admiration();

// --- Tabelle di puntatori a funzione [livello][emozione] ---
void (*reaction[3][NUM_EMOTIONS])() = {
    {annoyance, boredom, apprehension, serenity, pensiveness, distraction, interest, acceptance}, // lvl 1
    {anger, disgust, fear, joy, sadness, surprise, anticipation, trust},                          // lvl 2
    {rage, loathing, terror, ecstasy, grief, amazement, vigilance, admiration}                    // lvl 3
};

// --- Funzione per aggiornare le soglie di una singola emozione ---
void thresholds_check(int x) {
    float IE_adjust = (personality[0] == 'E') ? 0.1 : 0.0;

    t1 = THRESHOLD_1 - IE_adjust;
    t2 = THRESHOLD_2 - IE_adjust;
    t3 = THRESHOLD_3 - IE_adjust;

    // Limita le soglie tra 0 e 1
    if (t1 < 0) t1 = 0; 
    if (t1 > 1) t1 = 1;
    if (t2 < 0) t2 = 0; 
    if (t2 > 1) t2 = 1;
    if (t3 < 0) t3 = 0; 
    if (t3 > 1) t3 = 1;

    // Aggiorna il livello della singola emozione
    if (emotion[x] < t1) emotion_th[x] = 0;
    else if (emotion[x] < t2) emotion_th[x] = 1;
    else if (emotion[x] < t3) emotion_th[x] = 2;
    else emotion_th[x] = 3;
}

// --- Funzione principale di controllo: esprime solo l'emozione dominante ---
void update_all_thresholds(void) {
    // 1. Aggiorna tutte le soglie
    for (int i = 0; i < NUM_EMOTIONS; i++) {
        thresholds_check(i);
    }

    //surprise OVERRIDE
    if (emotion[5]>t3){ 
        emotion_th[5]=0;
        reaction[2][5]();
        emotion[5]=0;
    }else if (emotion[5]>t2){
        emotion_th[5]=0;
        reaction[1][5]();
        emotion[5]=0;
    }else if (emotion[5]>t1){
        emotion_th[5]=0;
        reaction[0][5]();
        emotion[5]=0;
    }else{
        emotion[5]=0;
    }

    // 2. Trova emozione con valore più alto
    int strongest_index = -1;
    int strongest_lvl = 0;
    float strongest_value = -1.0f;

    for (int i = 0; i < NUM_EMOTIONS; i++) {
        if (emotion_th[i] > 0) { // solo sopra soglia
            if (emotion[i] > strongest_value) {
                strongest_value = emotion[i];
                strongest_index = i;
                strongest_lvl = emotion_th[i];
        }
    }
    }

    //printf("Strongest emotion(%s) check: index=%d, level=%d, value=%.3f\n",emotion_name[strongest_index], strongest_index, strongest_lvl, strongest_value);

    // 3. Nessuna emozione sopra soglia
    if (strongest_index < 0) return;

    // 4. Esegui la reazione per la sola emozione dominante
    printf("\nEsprimo %s al livello %d\n", emotion_name[strongest_index], strongest_lvl);
    //printf("reaction[%d][%d] \n", strongest_lvl - 1, strongest_index);
    if (reaction[strongest_lvl - 1][strongest_index]) {
        //printf("Chiamo la reazione...\n");
        reaction[strongest_lvl - 1][strongest_index]();
    }

    vTaskDelay(pdMS_TO_TICKS(500)); // Pausa per evitare reazioni multiple immediate

    // 5. Reset emozione e livello espressi
    emotion[strongest_index] = 0;
    emotion_th[strongest_index] = 0;
}

#include "threshold_control.h"
#include "main.h"
#include "globals.h"
#include "serial.h" 
#include "idle.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>


// --- Include delle reazioni ---
#include "emotions/anger.h"
#include "emotions/fear.h"
#include "emotions/sadness.h"
#include "emotions/disgust.h"
#include "emotions/joy.h"
#include "emotions/surprise.h"
#include "emotions/trust.h"
#include "emotions/anticipation.h"

float t1;
float t2;
float t3;

int emotion_th[8]={0,0,0,0,0,0,0,0};


// --- Reazioni definite altrove ---
void annoyance(); void anger(); void rage();
void interest(); void anticipation(); void vigilance();
void boredom(); void disgust(); void loathing();
void apprehension(); void fear(); void terror();
void serenity(); void joy(); void ecstasy();
void pensiveness(); void sadness(); void grief();
void distraction(); void surprise(); void amazement();
void acceptance(); void trust(); void admiration();

// --- Tabelle di puntatori a funzione [livello][emozione] ---
void (*reaction[3][NUM_EMOTIONS])() = {
    {annoyance, interest, boredom, apprehension, serenity, pensiveness, distraction, acceptance}, // lvl 1
    {anger, anticipation, disgust, fear, joy, sadness, surprise, trust},                          // lvl 2
    {rage, vigilance, loathing, terror, ecstasy, grief, amazement, admiration}                    // lvl 3
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
    if (emotion[6]>t3){ 
        emotion_th[6]=0;
        reaction[2][6]();
        emotion_running_set(false);
        emotion[6]=0;
    }else if (emotion[6]>t2){
        emotion_th[6]=0;
        reaction[1][6]();
        emotion_running_set(false);
        emotion[6]=0;
    }else if (emotion[6]>t1){
        emotion_th[6]=0;
        reaction[0][6]();
        emotion_running_set(false);
        emotion[6]=0;
    }else{
        emotion[6]=0;
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
    //printf("\nExpressing %s at level %d:", emotion_name[strongest_index], strongest_lvl);
    if (reaction[strongest_lvl - 1][strongest_index]) {
        //printf("Chiamo la reazione...\n");
        reaction[strongest_lvl - 1][strongest_index]();
        emotion_running_set(false);
        idle_timer_reset();

    }

    usleep(500*1000); // Pausa per evitare reazioni multiple immediate

    // 5. Reset emozione e livello espressi
    emotion[strongest_index] = 0;
    emotion_th[strongest_index] = 0;
}


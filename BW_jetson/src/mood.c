#include "mood.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>






//Mood Scaler
void* mood_scaler_task(void* arg) {
    float mood_duration = MOOD_DEC_TIME;
    float mood_step= 0.1f;
    float mood_dec = 1.0f / (mood_duration / mood_step);

    while (1) {
        usleep((useconds_t)(mood_step * 1e6));

        pthread_mutex_lock(&mood_mutex);
        if (mood_int > 0.0f) {
            mood_int -= mood_dec;
            if (mood_int < 0.0f) 
                mood_int = 0.0f;
        }
        pthread_mutex_unlock(&mood_mutex);
        //printf("\nmood value: %.2f",mood_int);
    }
    return NULL;
}

//Mood modifications
void mood_effect(char mood_valence, float delta_mood){
    pthread_mutex_lock(&mood_mutex);

    if(mood_int == 0.0f){
        mood = mood_valence;
        mood_int += delta_mood;  
        printf("Mood set to %c, increased by %.2f, new mood value: %.2f\n\n", mood_valence, delta_mood, mood_int);
    } else if(mood == mood_valence){
        // if mood is aligned, intensity increases
        mood_int += delta_mood * (1.0f - mood_int); // max 1
        if(mood_int > 1.0f) mood_int = 1.0f;
        printf("Mood %c increased by %.2f, new mood value: %.2f\n\n", mood_valence, delta_mood*(1.0f-mood_int), mood_int);
    } else {
        // if mood is different, intensity decreases
        mood_int -= delta_mood * mood_int;
        if(mood_int < 0.0f) mood_int = 0.0f;
        printf("Mood %c decreased by %.2f, new mood value: %.2f\n\n", mood_valence, delta_mood*mood_int, mood_int);
    }

    pthread_mutex_unlock(&mood_mutex);
}

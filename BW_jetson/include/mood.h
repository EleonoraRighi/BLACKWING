#ifndef MOOD_H
#define MOOD_H

#include <pthread.h>  

#define MOOD_DEC_TIME 200.0f

extern pthread_mutex_t mood_mutex;
extern char mood;
extern float mood_int;


void* mood_scaler_task(void* arg);
void mood_effect(char mood_valence, float delta_mood);

#endif
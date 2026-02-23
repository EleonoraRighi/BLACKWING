#ifndef MAIN_H
#define MAIN_H

#include "stdint.h"
#include "stdbool.h"

#define TIMEOUT_MS     5000   // timeout = 5 seconds
#define MOOD_DEC_TIME  60     //seconds
#define NUM_EMOTIONS   8

extern char personality[16];
extern char mood;
extern float mood_int;
extern float emotion[8];
extern const char *emotion_name[8];
extern int emotion_th[8];



extern volatile uint64_t time_since_last_uart ;

void mood_manager(char mood, bool increase, float value);



#endif

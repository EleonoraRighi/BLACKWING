#ifndef IDLE_H
#define IDLE_H

#include <time.h>
#include "mood.h"

#define IDLE_TIME_1 0.25*MOOD_DEC_TIME
#define IDLE_TIME_2 0.30*MOOD_DEC_TIME
#define IDLE_TIME_3 0.40*MOOD_DEC_TIME



//-----------idle timer-------------------------------------------------------------
struct timespec idle_timer_start;
struct timespec idle_now;

void idle_timer_reset();
double idle_timer_elapsed();
//----------------------------------------------------------------------------------


void idle (void);

void idle_1(void);
void idle_2(void);
void idle_3(void);
void bow(void);




#endif
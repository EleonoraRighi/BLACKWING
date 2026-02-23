#ifndef GLOBALS_H
#define GLOBALS_H

#define NUM_EMOTIONS 8
#define SENSOR_WAIT 5000*1000



#include <stdbool.h>
#include <pthread.h>




extern char personality[16];   // dichiarazione
extern float emotion[8];
extern const char *emotion_name[8];
extern int emotion_th[8];
extern bool emotion_running;
extern pthread_mutex_t emotion_running_mutex;
bool emotion_running_get(void);
void emotion_running_set(bool value);


extern bool sensor_go;
extern pthread_mutex_t sensor_go_mutex;
extern pthread_cond_t sensor_go_cond;


extern pthread_mutex_t print_mutex;

extern struct timespec local_timer_start;
extern struct timespec local_now;

void local_timer_reset(void);
double local_timer_elapsed(void);

#endif

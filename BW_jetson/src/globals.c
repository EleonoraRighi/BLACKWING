#include "globals.h"
#include <ctype.h>
#include <time.h>
#include <pthread.h>

bool emotion_running = true;
pthread_mutex_t emotion_running_mutex = PTHREAD_MUTEX_INITIALIZER;

//-------------local timers---------------------------------------------------------------
struct timespec local_timer_start;
struct timespec local_now;

bool emotion_running_get(void) {
    bool value;
    pthread_mutex_lock(&emotion_running_mutex);
    value = emotion_running;
    pthread_mutex_unlock(&emotion_running_mutex);
    return value;
}

void emotion_running_set(bool value) {
    pthread_mutex_lock(&emotion_running_mutex);
    emotion_running = value;
    pthread_mutex_unlock(&emotion_running_mutex);
}


void local_timer_reset() {
    clock_gettime(CLOCK_MONOTONIC, &local_timer_start);
}

double local_timer_elapsed() {
    clock_gettime(CLOCK_MONOTONIC, &local_now);

    double elapsed = (local_now.tv_sec - local_timer_start.tv_sec) +
                     (local_now.tv_nsec - local_timer_start.tv_nsec) / 1e9;
    return elapsed;
}



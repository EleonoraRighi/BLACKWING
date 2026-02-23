#include "emotions/idle.h"
#include "globals.h"
#include "main.h"
#include "serial.h" 
#include "threshold_control.h"
#include "tracks_manager.h"
#include "math.h"
#include "mood.h"
#include "disgust.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


static char previous_action[16];

struct timespec idle_timer_start; 
struct timespec idle_now;

void idle_timer_reset() {
    clock_gettime(CLOCK_MONOTONIC, &idle_timer_start);
}
double idle_timer_elapsed() {
        clock_gettime(CLOCK_MONOTONIC, &idle_now);

    double elapsed = (idle_now.tv_sec - idle_timer_start.tv_sec) +
                     (idle_now.tv_nsec - idle_timer_start.tv_nsec) / 1e9;
    return elapsed;
}


void idle(void) {
    static struct timespec last_exec = {0, 0};
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    //printf("\n time since last action: %.2f ", idle_timer_elapsed());

    double since_last =
        (now.tv_sec - last_exec.tv_sec) +
        (now.tv_nsec - last_exec.tv_nsec) / 1e9;

    double t = idle_timer_elapsed();

    if (t < IDLE_TIME_1) {
        if (since_last >= 5) {
            last_exec = now; 
            idle_1();
            usleep(5*1000);
            
        }
    } else if (t < IDLE_TIME_2) {
        if (since_last >= 30) {
            last_exec = now;
            idle_2();
            usleep(500*1000);
        }
    } else if (t < IDLE_TIME_3) {
        if (since_last >= 0.5*MOOD_DEC_TIME) {  
            last_exec = now;
            idle_3();
            usleep(500*1000);
        }
    } else {
        idle_timer_reset();
        printf("\nNothing is happening and Blackwing is getting bored...\n");
        if(personality[0]=='E') boredom();
        usleep(500*1000);
    }
}


void idle_1(void){
    
    send_command("idle1\n");
    //play_sound("idle1.wav"); 
    usleep(50*1000);
}
void idle_2(void){
    emotion_running_set(true);
    send_command("idle2\n"); 
    play_sound("idle2.wav");
    usleep(50*1000);
    emotion_running_set(false);
}
void idle_3(void){
    emotion_running_set(true);
    send_string_arduino("START\n");
    send_arduino(0,0,-0.2);
    usleep(6000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    update_all_thresholds();
    usleep(50*1000); 
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void bow(void){
    emotion_running_set(true);
    send_command("bow\n"); 
    send_string_arduino("START\n");
    send_arduino(-0.3,0,0);
    usleep(2000*1000);
    send_arduino(0,0,0);
    usleep(4000*1000);
    send_arduino(0.3,0,0);
    usleep(2000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
    
    usleep(50*1000);
}

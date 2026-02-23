#include "emotions/surprise.h"
#include "globals.h"
#include "main.h"
#include "serial.h" 
#include  "mood.h"
#include "tracks_manager.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>


static char previous_action[16];
static float beta = 0;
static float inclination;
static float alpha;
static bool increase;
static float JP_scaler=1;

struct timespec local_timer_start;
struct timespec local_now;

//void mood_manager(char mood, bool increase, float value){}

int surprise_init(void)
{
    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.2;
    else if (personality[3] == 'P') alpha = 0.3;
    else printf("\nError in computation of alpha for surprise");

    if (personality[3] == 'J') JP_scaler=1.3;
    else if (personality[3] == 'P') JP_scaler=1;
    else  printf("\nError in computation of JP_scaler for surprise");
    return 0;
}

void surprise_modify(float action, const char *action_name){
    if(strcmp(previous_action,action_name)) beta++;
    else beta=0;

    if(strcmp(action_name,"noise")==0){
            emotion[6]=0.5*JP_scaler-beta/10;       
    }
    if(strcmp(action_name,"like")==0 && (strcmp(previous_action,"hit")!=0 && strcmp(previous_action,"like")!=0) && beta<1){
            emotion[6]=0.8*JP_scaler;        
    }
    if(strcmp(action_name,"closeness")==0 && (strcmp(previous_action,"hit")!=0 && strcmp(previous_action,"caress")!=0 && strcmp(previous_action,"closeness")!=0 ) && beta<1){
            emotion[6]=0.65*JP_scaler;       
    }
    if(strcmp(action_name,"hit")==0 && strcmp(previous_action,"caress")==0 && beta<1){
            emotion[6]=0.65*JP_scaler;     
    }   
    
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    if (emotion[6]<=0){
        emotion[6]=0;
    }else if (emotion[6]>=1){
        emotion[6]=1;
    }

}



void distraction(void){
    emotion_running_set(true);
    printf("\nExpressing DISTRACTION\n");
    send_command("distraction\n");
    play_sound("distraction.wav");
    usleep(6000*1000);
    
    if(personality[3]=='J') mood_effect('n',0.1);
    else if (personality[3]=='P') mood_effect('p',0.1);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void surprise(void){
    emotion_running_set(true);
    printf("\nExpressing SURPRISE\n");
    send_command("surprise\n");
    
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    play_sound("surprise.wav");
    send_arduino(0,0.5,0);
    usleep(700*1000);
    send_arduino(0,0,0);
    usleep(5000*1000);
    send_arduino(0,-0.1,0);
    usleep(1000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    if(personality[3]=='J') mood_effect('n',0.2);
    else if (personality[3]=='P') mood_effect('p',0.2);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void amazement(void){
    emotion_running_set(true);
    printf("\nExpressing AMAZEMENT\n");
    send_command("amazement\n");
    play_sound("amazement.wav");
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    play_sound("amazement.wav");
    send_arduino(0,0.3,0);
    usleep(2000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('p',0.5);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}


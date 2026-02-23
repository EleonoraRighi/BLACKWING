#include "emotions/disgust.h"
#include "globals.h"
#include "main.h"
#include "serial.h" 
#include "mood.h"
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
static float x;


//void mood_manager(char mood, bool increase, float value){}

int disgust_init(void)
{
    //printf("Anger initialized\n");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.2;
    else if (personality[3] == 'P') alpha = 0.3;
    else printf("\nError in computation of alpha for disgust");
    return 0;
}

void disgust_modify(float action, const char *action_name){

    if (!strcmp(action_name,"dislike")){
        if(!strcmp(previous_action,"closeness")){
            action=0.70;
        }
    }else if (!strcmp(action_name,"closeness")){
        if(!strcmp(previous_action,"dislike")){
            action=0.90;
        }
    }else{
        action=0;
    }

    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[2]=action;

    }
    else if (mood=='p'||mood=='P'){
        emotion[2]=action;
    }
    else {
        printf("\nQualcosa è andato storto con anger nel calcolo del valore di emotion[2]\n");
    }
    

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[2]<=0){
        emotion[2]=0;
    }else if (emotion[2]>=1){
        emotion[2]=1;
    }

    //printf("\nDisgust updated to %.2f", emotion[0]);
}






void boredom(void){
    emotion_running_set(true);
    printf("\nExpressing BOREDOM\n");
    send_command("boredom\n");
    play_sound("boredom.wav");
    //wheels command
    send_string_arduino("START\n");
    send_arduino(0,0.1,0);
    usleep(2000*1000);
    send_arduino(0,0,0);
    usleep(5000*1000);
    send_arduino(0,-0.1,0);
    usleep(2000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.1);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void disgust(void){
    emotion_running_set(true);
    printf("\nExpressing DISGUST\n");
    send_command("disgust\n");
    play_sound("disgust.wav");
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    
    send_arduino(0,0,0.3);
    usleep(500*1000);
    send_arduino(0,0,0);
    usleep(7000*1000);
    send_arduino(0,0,-0.1);
    usleep(1500*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.3);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void loathing(void){
    emotion_running_set(true);
    printf("\nExpressing LOATHING\n");
    send_command("loathing\n");
    play_sound("loathing.wav");
    //wheels commands
    send_string_arduino("START\n");
    usleep(1200*1000);
    
    send_arduino(0,0.4,0.2);
    usleep(1700*1000);
    send_arduino(0,0.2,0.1);
    usleep(50*1000);
    send_arduino(0,0,0);
    usleep(3000*1000);
    send_arduino(0,0,-0.2);
    usleep(2000*1000);
    send_arduino(0,-0.2,0);
    usleep(10000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.6);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}

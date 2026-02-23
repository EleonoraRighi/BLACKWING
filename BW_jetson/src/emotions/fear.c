#include "emotions/fear.h"
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
float (*f)(float);    //chosen function (depending on S or N)
static float fS(float x) {
    return (0.4*expf(x));      // linear
}
static float fN(float x) {
    return (0.4*expf(2*x));      // exponential
}
static float derivative(float (*f)(float), float x) {
    float h = 0.0001;
    return (f(x + h) - f(x)) / h;
}
struct timespec local_timer_start;
struct timespec local_now;

//void mood_manager(char mood, bool increase, float value){}

int fear_init(void)
{
    //printf("Anger initialized\n");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.5;
    else if (personality[3] == 'P') alpha = 0.3;
    else printf("\nError in computation of alpha for fear");

    if (personality[1] == 'S') f=fS;
    else if (personality[1] == 'N') f=fN;
    else  printf("\nError in computation of inclination for fear");
    //printf("\nAnger parameters are initialized\n");
    return 0;
}

void fear_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        beta=0;
    } else {
        printf("\nQualcosa è andato storto con fear nel calcolo di gamma\n");
    }

    inclination = derivative(f,beta);
    //printf("\ninclination= %.2f", inclination);


    if(strcmp("hug",action_name)==0||strcmp("caress",action_name)==0||strcmp("sorrow",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[3]+=( emotion[3]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+inclination));
        increase=true;

    }
    else if (mood=='p'||mood=='P'){
        emotion[3]+=( emotion[3]*alpha+ (fabsf(1-alpha))*(action*(1-mood_int)+inclination) ); 
        increase=false;
    }
    else {
        printf("\nQualcosa è andato storto con fear nel calcolo del valore di emotion[0]\n");
    }
    

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[3]<=0){
        emotion[3]=0;
    }else if (emotion[3]>=1){
        emotion[3]=1;
    }

    //printf("\nAnger updated to %.2f", emotion[0]);
}


void apprehension(void){
    emotion_running_set(true);
    printf("\nExpressing APPREHENSION\n");
    send_command("apprehension\n");
    
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    play_sound("apprehension.wav");
    send_arduino(0,0.1,0);
    usleep(4000*1000);
    send_arduino(0,0,0);
    usleep(5000*1000);
    send_arduino(0,-0.1,0);
    usleep(4000*1000);
    send_arduino(0,0,0);
    
    mood_effect('n',0.1);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void fear(void){
    emotion_running_set(true);
    printf("\nExpressing FEAR\n");
    send_command("fear\n");
    play_sound("fear.wav");
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    
    send_arduino(0,0.5,0);
    usleep(2750*1000);
    send_arduino(0,0.3,0);
    usleep(50*1000);
    send_arduino(0,0,0);
    usleep(5000*1000);
    send_arduino(0,-0.2,0);
    usleep(7000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.3);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void terror(void){
    emotion_running_set(true);
    printf("\nExpressing TERROR\n");
    send_command("terror\n");
    play_sound("terror.wav");
    //wheels commands
    send_string_arduino("START\n");
    usleep(800*1000);
    
    send_arduino(0,0.4,0);
    usleep(1000*1000);
    send_arduino(0,0,0.5);
    usleep(1300*1000);
    send_arduino(0,0,0);
    usleep(4000*1000);
    send_arduino(0,0,0.1);
    usleep(3500*1000);
    send_arduino(0,0,0);
    usleep(1000*1000);
    send_arduino(0,-0.1,0);
    usleep(5000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.6);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}

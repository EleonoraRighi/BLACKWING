#include "emotions/trust.h"
#include "globals.h"
#include "main.h"
#include "mood.h"
#include "serial.h" 
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
    return (0.1*expf(x));      // linear
}
static float fN(float x) {
    return (0.05*expf(x));      // exponential
}
static float derivative(float (*f)(float), float x) {
    float h = 0.0001;
    return (f(x + h) - f(x)) / h;
}


//void mood_manager(char mood, bool increase, float value){}

int trust_init(void)
{
    //printf("Anger initialized\n");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.2;
    else if (personality[3] == 'P') alpha = 0.3;
    else printf("\nError in computation of alpha for trust");

    if (personality[1] == 'S') f=fS;
    else if (personality[1] == 'N') f=fN;
    else  printf("\nError in computation of inclination for trust");
    //printf("\nAnger parameters are initialized\n");
    return 0;
}

void trust_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        beta=0;
    } else {
        printf("\nQualcosa è andato storto con trust nel calcolo di gamma\n");
    }

    inclination = derivative(f,beta);
    //printf("\ninclination= %.2f", inclination);

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[7]<=0){
        emotion[7]=0;
    }else if (emotion[7]>=1){
        emotion[7]=1;
    }

    //printf("\nAnger updated to %.2f", emotion[0]);
}





void acceptance(void){
    emotion_running_set(true);
    printf("\nExpressing ACCEPTANCE\n");
    send_command("acceptance\n");
    usleep(6000*1000);
    
    mood_effect('p',0);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void trust(void){
    emotion_running_set(true);
    printf("\nExpressing TRUST\n");
    send_command("trust\n");
    usleep(6000*1000);
    
    mood_effect('p',0);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void admiration(void){
    emotion_running_set(true);
    printf("\nExpressing ADMIRATION\n");
    send_command("admiration\n");
    usleep(6000*1000);
    
    mood_effect('p',0);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}

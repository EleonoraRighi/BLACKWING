#include "emotions/anger.h"
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
    return (0.4*expf(3*x));      // linear
}
static float fN(float x) {
    return (0.45*expf(2*x));      // exponential
}
static float derivative(float (*f)(float), float x) {
    float h = 0.0001;
    return (f(x + h) - f(x)) / h;
}


struct timespec local_timer_start;
struct timespec local_now;


//void mood_manager(char mood, bool increase, float value){}

int anger_init(void)
{
    //printf("Anger initialized\n");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.5;
    else if (personality[3] == 'P') alpha = 0.1;
    else printf("\nError in computation of alpha for anger");

    if (personality[1] == 'S') f=fS;
    else if (personality[1] == 'N') f=fN;
    else  printf("\nError in computation of inclination for anger");
    //printf("\nAnger parameters are initialized\n");
    return 0;
}

void anger_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        beta=0;
    } else {
        printf("\nQualcosa è andato storto con anger nel calcolo di gamma\n");
    }

    inclination = derivative(f,beta);
    //printf("\ninclination= %.2f", inclination);


    if(strcmp("hug",action_name)==0||strcmp("caress",action_name)==0||strcmp("sorrow",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[0]+=( emotion[0]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+inclination));
        increase=true;

    }
    else if (mood=='p'||mood=='P'){
        emotion[0]+=( emotion[0]*alpha+ (fabsf(1-alpha))*(action*(1-mood_int)+inclination) ); 
        increase=false;
    }
    else {
        printf("\nQualcosa è andato storto con anger nel calcolo del valore di emotion[0]\n");
    }
    

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[0]<=0){
        emotion[0]=0;
    }else if (emotion[0]>=1){
        emotion[0]=1;
    }

    //printf("\nAnger updated to %.2f", emotion[0]);
}

void annoyance(void){
    emotion_running_set(true);
    printf("\nExpressing ANNOYANCE\n");
    send_command("annoyance\n");
    usleep(1000*1000);
    play_sound("annoyance.wav");
    usleep(5000*1000);
    
    mood_effect('n',0.1);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void anger(void){
    emotion_running_set(true);
    printf("\nExpressing ANGER\n");
    send_command("anger\n");
    local_timer_reset();
    play_sound("anger.wav");
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    
    while(local_timer_elapsed()<4.5){
        send_arduino(0.4,0,0);
        usleep(100*1000);
        send_arduino(-0.4,0,0);
        usleep(100*1000);
    }
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
   
    mood_effect('n',0.3);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void rage(void){
    emotion_running_set(true);
    printf("\nExpressing RAGE\n");
    send_command("rage\n");
    local_timer_reset();
    play_sound("rage.wav");
    //wheels commands
    send_string_arduino("START\n");
    while(local_timer_elapsed()<2.0){
        send_arduino(0.4,0,0);
        usleep(100*1000);
        send_arduino(-0.4,0,0);
        usleep(100*1000);
    }
    send_arduino(0,-0.4,0);
    usleep(1000*1000);
    send_arduino(0,-0.2,0);
    usleep(100*1000);
    send_arduino(0,0,0);
    local_timer_reset();
    send_string_arduino("START\n");
    while(local_timer_elapsed()<3.0){
        send_arduino(0.4,0,0);
        usleep(100*1000);
        send_arduino(-0.4,0,0);
        usleep(100*1000);
    }
    send_arduino(0,0,0);
    usleep(3000*1000);
    send_arduino(0,0.3,0);
    usleep(1250*1000);
    send_arduino(0,0.1,0);
    usleep(50*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.6);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}


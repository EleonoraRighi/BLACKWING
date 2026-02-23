#include "emotions/joy.h"
#include "globals.h"
#include "serial.h"
#include "main.h" 
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
    return (0.15*expf(2*x));      // linear
}
static float fN(float x) {
    return (0.20*expf(2*x));      // exponential
}
static float derivative(float (*f)(float), float x) {
    float h = 0.0001;
    return (f(x + h) - f(x)) / h;
}

struct timespec local_timer_start;
struct timespec local_now;




//void mood_manager(char mood, bool increase, float value){}

int joy_init(void)
{
    //printf("Anger initialized\n");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.7;
    else if (personality[3] == 'P') alpha = 0.1;
    else printf("\nError in computation of alpha for joy");

    if (personality[1] == 'S') f=fS;
    else if (personality[1] == 'N') f=fN;
    else  printf("\nError in computation of inclination for joy");
    //printf("\nAnger parameters are initialized\n");
    return 0;
}

void joy_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        beta=0;
    } else {
        printf("\nQualcosa è andato storto con joy nel calcolo di gamma\n");
    }

    inclination = derivative(f,beta);
    //printf("\ninclination= %.2f", inclination);


    if(strcmp("hug",action_name)==0||strcmp("caress",action_name)==0||strcmp("sorrow",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[4]+=( emotion[4]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+inclination));
        increase=true;

    }
    else if (mood=='p'||mood=='P'){
        emotion[4]+=( emotion[4]*alpha+ (fabsf(1-alpha))*(action*(1-mood_int)+inclination) ); 
        increase=false;
    }
    else {
        printf("\nQualcosa è andato storto con joy nel calcolo del valore di emotion[0]\n");
    }
    

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[4]<=0){
        emotion[4]=0;
    }else if (emotion[4]>=1){
        emotion[4]=1;
    }

    //printf("\nAnger updated to %.2f", emotion[0]);
}




void serenity(void){
    emotion_running_set(true);
    printf("\nExpressing SERENITY\n");
    send_command("serenity\n");
    play_sound("serenity.wav");
    usleep(1000*1000);
    local_timer_reset();
    //wheels commands
    while(local_timer_elapsed()<6.0){
        send_string_arduino("START\n");
        send_arduino(0,0.1,0);
        usleep(1000*1000);
        send_arduino(0,0,0);
        usleep(100*1000);
        send_arduino(0,-0.1,0);
        usleep(1000*1000);
    }
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('p',0.1);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void joy(void){
    emotion_running_set(true);
    printf("\nExpressing JOY\n");
    send_command("joy\n");
    
    local_timer_reset();
    //wheels commands
    send_string_arduino("START\n");
    usleep(1000*1000);
    play_sound("joy.wav");
    while(local_timer_elapsed()<4.5){
        send_arduino(0,0,0.3);
        usleep(300*1000);
        send_arduino(0,0,-0.3);
        usleep(300*1000);
    }
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('p',0.3);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void ecstasy(void){
    emotion_running_set(true);
    printf("\nExpressing ECSTASY\n");
    send_command("ecstasy\n");
   
    //wheels commands
    usleep(1000*1000);
    play_sound("ecstasy.wav");
    send_string_arduino("START\n");
    send_arduino(0,0,-0.5);
    usleep(1900*1000);
    send_arduino(0,0,-0.2);
    usleep(100*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('p',0.6);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}

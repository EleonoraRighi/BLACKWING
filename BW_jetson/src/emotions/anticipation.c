#include "emotions/anticipation.h"
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
    return (0.1*expf(3*x));      // linear
}
static float fN(float x) {
    return (0.35*expf(2*x));      // exponential
}
static float derivative(float (*f)(float), float x) {
    float h = 0.0001;
    return (f(x + h) - f(x)) / h;
}

struct timespec local_timer_start;
struct timespec local_now;


//void mood_manager(char mood, bool increase, float value){}

int anticipation_init(void)
{
    //printf("Anticipation initialized\n");

    // Parametri in base alla personalità
    if (personality[3] == 'J') alpha = 0.7;
    else if (personality[3] == 'P') alpha = 0.2;
    else  printf("\nError in computation of alpha for anticipation");

    if (personality[1] == 'S') f=fS;
    else if (personality[1] == 'N') f=fN;
    else return printf("\nError in computation of inclination for anticipation");;
    //printf("\nAnger parameters are initialized\n");
    return 0;
    
}

void anticipation_modify(float action, const char *action_name){
    //tenere conto delle ripetizioni di action (gamma)
    if (strcmp(previous_action, action_name)==0){
        beta+=1;
    } else if (strcmp(previous_action, action_name)!=0){
        beta=0;
    } else {
        printf("\nQualcosa è andato storto con anticipation nel calcolo di gamma\n");
    }

    inclination = derivative(f,beta);
    //printf("\ninclination= %.2f", inclination);


    if(strcmp("hug",action_name)==0||strcmp("caress",action_name)==0||strcmp("sorrow",action_name)==0){
        beta=-beta;
    }
    snprintf(previous_action, sizeof(previous_action), "%s", action_name);

    //definizione funzione di incremento
    if( mood=='n'||mood=='N'){
        emotion[1]+=( emotion[1]*alpha + (fabsf((1-alpha)))*(action*(mood_int)+inclination));
        increase=true;

    }
    else if (mood=='p'||mood=='P'){
        emotion[1]+=( emotion[1]*alpha+ (fabsf(1-alpha))*(action*(1-mood_int)+inclination) ); 
        increase=false;
    }
    else {
        printf("\nQualcosa è andato storto con anger nel calcolo del valore di emotion[1]\n");
    }
    

//    printf("emotion[0] at t: %.3f\n",emotion[0]);
    if (emotion[1]<=0){
        emotion[1]=0;
    }else if (emotion[1]>=1){
        emotion[1]=1;
    }

    //printf("\nAnger updated to %.2f", emotion[0]);
}



void interest(void){
    emotion_running_set(true);
    printf("\nExpressing INTEREST\n");
    send_command("interest\n");
    play_sound("interest.wav");
    //wheels commands
    send_string_arduino("START\n");
    send_arduino(0,-0.2,0);
    usleep(1000*1000);
    send_arduino(0,0,0);
    /*
    usleep(5000*1000);
    send_arduino(0,0.2,0);
    usleep(1000*1000);
    send_arduino(0,0,0);
    */
    send_string_arduino("STOP\n");
    
    mood_effect('p',0.1);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void anticipation(void){
    emotion_running_set(true);
    printf("\nExpressing ANTICIPATION\n");
    send_command("anticipation\n");
    
    //wheels commands
    send_string_arduino("START\n");
    usleep(500*1000);
    play_sound("anticipation.wav");
    send_arduino(0,-0.2,0);
    usleep(1000*1000);
    send_arduino(0,0,0);
    usleep(1000*1000);
    send_arduino(0,-0.2,0);
    usleep(1000*1000);
    send_arduino(0,0,0);
    /*
    usleep(3000*1000);
    send_arduino(0,0.2,0);
    usleep(2000*1000);
    send_arduino(0,0,0);
    */
    send_string_arduino("STOP\n");
    
    mood_effect('p',0.3);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}
void vigilance(void){
    emotion_running_set(true);
    printf("\nExpressing VIGILANCE\n");
    send_command("vigilance\n");
    play_sound("vigilance.wav");
    //wheels commands
    send_string_arduino("START\n");
    send_arduino(0,0.1,0);
    usleep(3000*1000);
    send_arduino(0,0,0);
    usleep(500*1000);
    send_arduino(0,0,0.4);
    usleep(250*1000);
    send_arduino(0,0,0);
    usleep(2000*1000);
    send_arduino(0,0,-0.4);
    usleep(500*1000);
    send_arduino(0,0,0);
    usleep(2000*1000);
    send_arduino(0,0,0.4);
    usleep(250*1000);
    send_arduino(0,0,0);
    usleep(5000*1000);
    send_arduino(0,-0.2,0);
    usleep(1000*1000);
    send_arduino(0,0,0);
    send_string_arduino("STOP\n");
    
    mood_effect('n',0.3);
    usleep(SENSOR_WAIT);
    emotion_running_set(false);
}

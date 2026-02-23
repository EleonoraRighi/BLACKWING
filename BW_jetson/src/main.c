#include "main.h"
#include "globals.h"
#include "serial.h" 
#include "mood.h"
#include "threshold_control.h"
#include "camera_interface.h"
#include "input_manager.h"
#include "closeness_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>


#include "emotions/anger.h"
#include "emotions/anticipation.h"
#include "emotions/disgust.h"
#include "emotions/fear.h"
#include "emotions/joy.h"
#include "emotions/sadness.h"
#include "emotions/surprise.h"
#include "emotions/trust.h"
#include "emotions/idle.h"

//#define ESP1_SERIAL_PORT "/dev/ttyUSB0"
//#define ARDUINO_SERIAL_PORT "/dev/ttyACM0"


#define RESET_INTERVAL 30  // seconds
#define SIGNAL_FILE "ipc/vision_signal.txt"


//Mutex
pthread_mutex_t mood_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t emotion_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global Variables
char personality[16];
char mood_str[16];
char mood;
char mood_int_str[16];
float mood_int = 1.0f;
const char *emotion_name[]={"Anger","Anticipation", "Disgust","Fear","Joy","Sadness","Surprise","Trust"};
float emotion[8]={0.1,0.0,0.1,0.1,0.1,0.1,0.0,0.0};
int i=0;

// Timer Variable
struct timespec timer_start;



//Timer
void timer_reset() {
    clock_gettime(CLOCK_MONOTONIC, &timer_start);
}
double timer_elapsed() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    double elapsed = (now.tv_sec - timer_start.tv_sec) +
                     (now.tv_nsec - timer_start.tv_nsec) / 1e9;
    return elapsed;
}

//sensor reading manager
bool sensor_go = false;
pthread_mutex_t sensor_go_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sensor_go_cond = PTHREAD_COND_INITIALIZER;

//funzione di attesa
void wait_for_sensors_go() {
    pthread_mutex_lock(&sensor_go_mutex);
    while (!sensor_go) {
        pthread_cond_wait(&sensor_go_cond, &sensor_go_mutex);
    }
    pthread_mutex_unlock(&sensor_go_mutex);
}
void sensors_start(void) {
    pthread_mutex_lock(&sensor_go_mutex);
    sensor_go = true;
    pthread_mutex_unlock(&sensor_go_mutex);
}
bool sensors_allowed(void) {
    if (!sensor_go) return false;
    if (emotion_running_get()) return false;
    return true;
}


//Initial input manager
static bool read_line_from_tty(char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) return false;

    int fd = open("/dev/tty", O_RDONLY);
    FILE *in = NULL;
    if (fd >= 0) {
        in = fdopen(fd, "r");
    }
    if (!in) {
        in = stdin; // fallback
    }

    if (fgets(buf, (int)buf_size, in) == NULL) {
        if (in != stdin) fclose(in);
        return false;
    }
    if (in != stdin) fclose(in);

    // strip newline
    buf[strcspn(buf, "\r\n")] = 0;
    return true;
}

bool ask_personality(void){

    printf("\n\033[36mHI! What is my personality type? (es. INTP, ENFP, ISTJ, ...)\033[0m\n");
    if (!read_line_from_tty(personality, sizeof(personality))) {
        printf("\n\033[1;31mError: no input available (stdin closed or no TTY).\033[0m\n");
        return false;
    }

    for (i = 0; i < strlen(personality); i++) {
        personality[i] = toupper(personality[i]);
    }

    //input control
    if (personality[0]=='I' || personality[0]=='E'){
        if (personality[1]=='S' || personality[1]=='N'){
            if (personality[2]=='T' || personality[2]=='F'){
                if (personality[3]=='J' || personality[3]=='P'){
                    return true;
                } else {
                    printf("\n\033[1;31mError: last letter must be either J or P.\033[0m\n");
                    return false;
                }
            } else {
                printf("\n\033[1;31mError: third letter must be either T or F.\033[0m\n");
                return false;
            }
        } else {
            printf("\n\033[1;31mError: second letter must be either S or N.\033[0m\n");
            return false;
        }
    } else {
        printf("\n\033[1;31mError: first letter must be either I or E.\033[0m\n");
        return false;
    }
}
bool ask_mood(void){

    printf("\n\033[36mWhat is my mood right now? (pos or neg)\033[0m\n");
    if (!read_line_from_tty(mood_str, sizeof(mood_str))) {
        printf("\n\033[1;31mError: no input available (stdin closed or no TTY).\033[0m\n");
        return false;
    }
    //input control
    if (mood_str[0]=='p' || mood_str[0]=='P'){
        strcpy(mood_str,"positive");
        return true;
    } 
    if (mood_str[0]=='n' || mood_str[0]=='N' ){
        strcpy(mood_str,"negative");
        return true;
    } else {
        printf("\n\033[1;31mError: Try answering by only choosing between pos or neg.\033[0m\n");
        return false;
                }

}
bool ask_mood_intensity(void){

    printf("\n\033[36mWhat is the intensity of the mood I'm feeling? (choose a number between 0 and 1, using up to 2 decimals)\033[0m\n");
    if (!read_line_from_tty(mood_int_str, sizeof(mood_int_str))) {
        printf("\n\033[1;31mError: no input available (stdin closed or no TTY).\033[0m\n");
        return false;
    }

    mood_int=atof(mood_int_str);
    //input control
    if (mood_int>=0 && mood_int<=1){
        return true;
    } else {
        printf("\n\033[1;31mError: The only acceptable values are between 0 and 1. eg 0.5.\033[0m\n");
        return false;
                }

}

//Emotion parameters initialization
void emotion_initializer(void){
//Initialization of the emotion array with fixed values, scaled based on mood and personality trait 3
    //multiplicative factor related to T/F personality trait (F are more sentimental, so they will be more influenced by mood)
    float F_scaler=1;
    if (personality[2]=='F')
        F_scaler=1.5;
    
    //Adjust emotions based on mood
    if (mood=='n'||mood=='N'){
        emotion[0]+=0.1*F_scaler;
        emotion[4]-=0.1*F_scaler;
        emotion[5]+=0.1*F_scaler;
    }else if (mood=='p' || mood=='P'){
        emotion[1]+=0.1*F_scaler;
        emotion[2]-=0.1*F_scaler;
        emotion[4]+=0.1*F_scaler;
        emotion[5]-=0.1*F_scaler;
        emotion[7]+=0.1*F_scaler;
    }
    //make sure all emotions are between 0 and 1
    for(int i=0;i<NUM_EMOTIONS;i++){
        if(emotion[i]<=0){
            emotion[i]=0;
        }else if(emotion[i]>=1){
            emotion[i]=1;
        }
    }
    
    printf("\n\033[36mSince my personality trait #3 is %c and my mood is %s\033[0m\n", personality[2], mood_str);
    usleep(500*1000);    //delay to allow terminal to sample

    //print emotions initial values
    for(i=0;i<NUM_EMOTIONS;i++){
        printf("\n\033[36m%s:%.3f\033[0m ", emotion_name[i], emotion[i]);
        usleep(10*1000); // to avoid watchdog reset
    }
    printf("\n");

    //initialize components
    printf("\n");
    if(anger_init()==0) printf("\033[90mAnger parameters initialized correctly\033[0m\n");
    if(anticipation_init()==0) printf("\033[90mAnticipation parameters initialized correctly\033[0m\n");
    if(disgust_init()==0) printf("\033[90mDisgust parameters initialized correctly\033[0m\n");
    if(fear_init()==0) printf("\033[90mFear parameters initialized correctly\033[0m\n");
    if(joy_init()==0) printf("\033[90mJoy parameters initialized correctly\033[0m\n");
    if(sadness_init()==0) printf("\033[90mSadness parameters initialized correctly\033[0m\n");
    if(surprise_init()==0) printf("\033[90mSurprise parameters initialized correctly\033[0m\n");
    if(trust_init()==0) printf("\033[90mTrust parameters initialized correctly\033[0m\n");
}





// Funzione callback nel main
void handle_signal(const char *source, const char *data) {
    if (!data || data[0] == '\0') return;

    printf("\n\033[90mInput received: '%s'\033[0m\n", data);

    /* ============================================================
       EMOTIONAL INPUTS (bloccati se emotion_running == true)
       ============================================================ */
    if (
        strcmp(data, "hit") == 0 ||
        strcmp(data, "like") == 0 ||
        strcmp(data, "caress") == 0 ||
        strcmp(data, "dislike") == 0 ||
        strcmp(data, "noise") == 0 ||
        strcmp(data, "closeness") == 0
    ) {
        if (emotion_running_get()) return;

        pthread_mutex_lock(&emotion_mutex);

        if (strcmp(data, "hit") == 0) {
            anger_modify(0.55, "hit");
            anticipation_modify(-0.2*5, "hit");
            disgust_modify(0, "hit");
            fear_modify(0.75, "hit");
            joy_modify(-0.77*5, "hit");
            sadness_modify(0.3, "hit");
            surprise_modify(0.65, "hit");
            trust_modify(0, "hit");

        } else if (strcmp(data, "like") == 0) {
            anger_modify(-0.55*5, "like");
            anticipation_modify(0.95, "like");
            disgust_modify(0, "like");
            fear_modify(-0.5*5, "like");
            joy_modify(0.5, "like");
            sadness_modify(-0.5*5, "like");
            surprise_modify(0.90, "like");
            trust_modify(0, "like");

        } else if (strcmp(data, "caress") == 0) {
            anger_modify(-0.4*5, "caress");
            anticipation_modify(0.05, "caress");
            disgust_modify(0, "caress");
            fear_modify(-0.65*5, "caress");
            joy_modify(0.5, "caress");
            sadness_modify(-0.55*5, "caress");
            surprise_modify(0, "caress");
            trust_modify(0, "caress");

        } else if (strcmp(data, "dislike") == 0) {
            anger_modify(0.35, "dislike");
            anticipation_modify(-0.73*5, "dislike");
            disgust_modify(0.65, "dislike");
            fear_modify(0.03, "dislike");
            joy_modify(-0.5*5, "dislike");
            sadness_modify(0.60, "dislike");
            surprise_modify(0, "dislike");
            trust_modify(0, "dislike");

        } else if (strcmp(data, "noise") == 0) {
            anger_modify(0.15, "noise");
            anticipation_modify(1.99, "noise");
            disgust_modify(0, "noise");
            fear_modify(0.5, "noise");
            joy_modify(-0.2*5, "noise");
            sadness_modify(0.45, "noise");
            surprise_modify(0.55, "noise");
            trust_modify(0, "noise");

        } else if (strcmp(data, "closeness") == 0) {
            int max_i = 0;
            for (int i = 1; i < NUM_EMOTIONS; i++) {
                if (emotion[i] > emotion[max_i]) max_i = i;
            }

            switch (max_i) {
                case 0: anger_modify(0.2, "closeness"); break;
                case 1: anticipation_modify(0.4, "closeness"); break;
                case 2: disgust_modify(0.7, "closeness"); break;
                case 3: fear_modify(0.45, "closeness"); break;
                case 4: joy_modify(0.25, "closeness"); break;
                case 5: sadness_modify(-0.6*5, "closeness"); break;
                case 6: surprise_modify(0, "closeness"); break;
                default: trust_modify(0, "closeness"); break;
            }
        }

        pthread_mutex_unlock(&emotion_mutex);
        for (int i = 0; i < NUM_EMOTIONS; i++) {
            printf("%s: %.3f\n", emotion_name[i], emotion[i]);
            usleep(10 * 1000);
        }
        update_all_thresholds();
        return;
    }

    /* ============================================================
       COMANDI SEMPRE ATTIVI (debug / azioni / servo)
       ============================================================ */

    
    if (strcmp(data, "test") == 0) {
        // test

    } else if (strcmp(data, "ang1") == 0) annoyance();
    else if (strcmp(data, "ang2") == 0) anger();
    else if (strcmp(data, "ang3") == 0) rage();

    else if (strcmp(data, "ant1") == 0) interest();
    else if (strcmp(data, "ant2") == 0) anticipation();
    else if (strcmp(data, "ant3") == 0) vigilance();

    else if (strcmp(data, "dis1") == 0) boredom();
    else if (strcmp(data, "dis2") == 0) disgust();
    else if (strcmp(data, "dis3") == 0) loathing();

    else if (strcmp(data, "fear1") == 0) apprehension();
    else if (strcmp(data, "fear2") == 0) fear();
    else if (strcmp(data, "fear3") == 0) terror();

    else if (strcmp(data, "joy1") == 0) serenity();
    else if (strcmp(data, "joy2") == 0) joy();
    else if (strcmp(data, "joy3") == 0) ecstasy();

    else if (strcmp(data, "sad1") == 0) pensiveness();
    else if (strcmp(data, "sad2") == 0) sadness();
    else if (strcmp(data, "sad3") == 0) grief();

    else if (strcmp(data, "sur1") == 0) distraction();
    else if (strcmp(data, "sur2") == 0) surprise();
    else if (strcmp(data, "sur3") == 0) amazement();

    else if (strcmp(data, "tr1") == 0) acceptance();
    else if (strcmp(data, "tr2") == 0) trust();
    else if (strcmp(data, "tr3") == 0) admiration();

    else if (strcmp(data, "idle1") == 0) idle_1();
    else if (strcmp(data, "idle2") == 0) idle_2();
    else if (strcmp(data, "bow") == 0) bow();
    

    if (data[0] == 'x') {
        float value = atof(&data[1]);
        send_string_arduino("START\n");
        usleep(500 * 1000);
        send_arduino(value, 0, 0);
        usleep(2000 * 1000);
        send_arduino(0, 0, 0);
        usleep(500 * 1000);
        send_string_arduino("STOP\n");

    } else if (data[0] == 'y') {
        float value = atof(&data[1]);
        send_string_arduino("START\n");
        usleep(500 * 1000);
        send_arduino(0, value, 0);
        usleep(2000 * 1000);
        send_arduino(0, 0, 0);
        usleep(500 * 1000);
        send_string_arduino("STOP\n");

    } else if (data[0] == 'z') {
        float value = atof(&data[1]);
        send_string_arduino("START\n");
        usleep(500 * 1000);
        send_arduino(0, 0, value);
        usleep(2000 * 1000);
        send_arduino(0, 0, 0);
        usleep(500 * 1000);
        send_string_arduino("STOP\n");

    } else {
        printf("Unknown command from %s: '%s'\n", source, data);
    }

    for (int i = 0; i < NUM_EMOTIONS; i++) {
        printf("%s: %.3f\n", emotion_name[i], emotion[i]);
        usleep(10 * 1000);
    }
}






int main(void) {
    printf("🔥\nMain started!\n");

    // Serial Initialization
    serial_esp1_fd = open_serial_esp(ESP1_SERIAL_PORT);  // ESP32 #1
    serial_esp2_fd = open_serial_esp(ESP2_SERIAL_PORT);  // ESP32 #2
    open_serial_arduino();

    if (serial_esp1_fd < 0 || serial_esp2_fd < 0) {
        fprintf(stderr, "Error opening ESP serials\n");
        return -1;
    }

    // Thread di lettura seriale per ciascun ESP
    serial_args_t esp1_args = { serial_esp1_fd, 1 };
    serial_args_t esp2_args = { serial_esp2_fd, 2 };

    pthread_t esp1_thread, esp2_thread;
    pthread_create(&esp1_thread, NULL, serial_read_thread, &esp1_args);
    pthread_create(&esp2_thread, NULL, serial_read_thread, &esp2_args);

    // Send initialization message to ESP (will start servo_init_all() and servo_manager_init() etc... on ESP)
    send_command("START\n");  
    
    usleep(3000*1000); //wait for response from serial


//-----------------------------------------------------------------  


   //Initialize parameters through user input
    // First Question
    bool verify_1=false;
    while(verify_1==false){
        verify_1=ask_personality();
        if (verify_1==false){
            printf("Seems like there was an error in the personality type. Please try again.\n");
            usleep(500*1000);   //500 ms
        }
    }
    printf("\nYou chose: %s\n", personality);

    // Second Question
    bool verify_2=false;
    while(verify_2==false){
        verify_2=ask_mood();
        if (verify_2==false){
            printf("Seems like there was an error in the mood choice. Please try again.\n");
            usleep(500*1000);
        }
    }
    mood=mood_str[0];
    printf("\nYou chose: %s mood\n", mood_str);

    // Third Question
    bool verify_3=false;
    while(verify_3==false){
        verify_3=ask_mood_intensity();
        if (verify_3==false){
            printf("Seems like there was an error in the mood intensity choice. Please try again.\n");
            usleep(500*1000);
        }
    }
    printf("\nYou chose : %.0f%% %s mood\n", mood_int*100, mood_str);
    
    // Answer summary
    printf("\nAnswer Summary:\n");
    printf("Personality: %s\n", personality);
    printf("Mood: %s\n", mood_str);
    printf("Intensity of mood:%.2f\n", mood_int);

    usleep(500*1000); 
//-----------------------------------------------------------------
   


    //initialize emotion values
    emotion_initializer();
    usleep(500*1000);

    //create mood_scaler task
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, mood_scaler_task, NULL);
    usleep(500*1000);

    init_sonar();
    input_initialize(handle_signal);  // passa la callback

//-----------------------------------------------------------------

    //Ready to start:

    printf("\n\033[38;5;214mIt's SHOWTIME!🎭\033[0m\n");
    idle_timer_reset();
    emotion_running_set(false);

    sensors_start();


    while(1){
        if (!emotion_running_get()){
            //idle();
            //printf("\nListening and Idling...");
            fflush(stdout);
        }
        usleep(500*1000);
    }
    return 0;
}




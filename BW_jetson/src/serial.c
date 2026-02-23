#include "globals.h"
#include "serial.h"  
#include "closeness_manager.h"
#include "touch_manager.h"        
#include "main.h"
#include <stdio.h>           
#include <string.h>          
#include <unistd.h>          
#include <fcntl.h>           
#include <errno.h>           
#include <pthread.h>         
#include <termios.h>      

// SERIALI
int serial_esp1_fd = -1;    // ESP32
int serial_esp2_fd = -1;    // ESP32 for sonars
int serial_arduino_fd = -1;    // Arduino



//Thread for serial communication with ESP32
void* serial_read_thread(void* arg) {
    serial_args_t* args = (serial_args_t*)arg;
    char buf[1024];

    static bool warned = false;

    while (1) {
        ssize_t n = read(args->fd, buf, sizeof(buf) - 1);
        if (n <= 0) continue;

        buf[n] = '\0';

        // stampa log NON sensori (sempre)
        pthread_mutex_lock(&print_mutex);
        if (strstr(buf, "SONAR") == NULL &&
            strstr(buf, "caress") == NULL &&
            strstr(buf, "hit") == NULL) {
            printf("%s", buf);
            fflush(stdout);
        }
        pthread_mutex_unlock(&print_mutex);

        // ðŸ”’ BLOCCO SENSORI DURANTE EMOZIONI
        if (emotion_running_get()) {
            if (!warned) {
                //printf("\033[2m[SENSORS PAUSED â€“ emotion running]\033[0m\n");
                warned = true;
            }
            continue;   // â— salta SOLO sensori
        }


        warned = false;

        // âœ… sensori attivi
        if (strstr(buf, "SONAR") != NULL)
            closeness_manager(buf);

        touch_manager(buf);
    }
}



//Thread for serial communication with Arduino
void* serial_read_arduino_thread(void* arg) {
    static char buf[512];
    static int i = 0;
    while (1) {
        char c;
        ssize_t n = read(serial_arduino_fd, &c, 1);
        if (n > 0) {
            if (c == '\n') {
                buf[i] = '\0';
                
                pthread_mutex_lock(&print_mutex);

                // Prova a riconoscere JSON minimale
                float speed_x = 0, speed_y = 0, angular_speed = 0;
                if (strstr(buf, "{") && strstr(buf, "speed_x") && strstr(buf, "speed_y") && strstr(buf, "angular_speed")) {
                    // Parsing manuale
                    sscanf(buf, "{\"speed_x\":%f,\"speed_y\":%f,\"angular_speed\":%f}", &speed_x, &speed_y, &angular_speed);
                    //printf("[Arduino â†’ Jetson] JSON ricevuto: speed_x=%.3f, speed_y=%.3f, angular_speed=%.3f\n", speed_x, speed_y, angular_speed);
                } else {
                    // Stringa normale
                    if(!strcmp(buf,"START") && !strcmp(buf,"STOP"))
                        printf("[Arduino â†’ Jetson] Stringa: %s\n", buf);
                }

                fflush(stdout);
                pthread_mutex_unlock(&print_mutex);

                i = 0;
            } else if (i < sizeof(buf) - 1) {
                buf[i++] = c;
            }
        } else {
            usleep(1000);
        }
    }
    return NULL;
}

//To send commands to ESP32 (String)
void send_command(const char* cmd) {
    if (serial_esp1_fd < 0) return;
    ssize_t w = write(serial_esp1_fd, cmd, strlen(cmd));
    if (w < 0) {
        fprintf(stderr, "[Jetson] Error sending string to ESP32: %s\n", strerror(errno));
    } else {
        pthread_mutex_lock(&print_mutex);
        //printf("[Jetson â†’ ESP32] %s", cmd);
        fflush(stdout);
        pthread_mutex_unlock(&print_mutex);
    }
}

//To send commands to Arduino (Json and String)
void send_arduino(float speed_x, float speed_y, float angular_speed) {
    if (serial_arduino_fd < 0) return;
    
    // crea il JSON come stringa
    char json_buf[128];
    snprintf(json_buf, sizeof(json_buf),
             "{\"speed_x\":%.3f,\"speed_y\":%.3f,\"angular_speed\":%.3f}\n",
             speed_x, speed_y, angular_speed);

    // invia la stringa all'Arduino
    ssize_t w = write(serial_arduino_fd, json_buf, strlen(json_buf));
    if (w < 0) {
        fprintf(stderr, "[Jetson] Error sending JSON string to Arduino: %s\n", strerror(errno));
    } else {
        pthread_mutex_lock(&print_mutex);
        //printf("[Jetson â†’ Arduino] %s", json_buf);
        //fflush(stdout);
        pthread_mutex_unlock(&print_mutex);
    }
}
void send_string_arduino(const char* cmd_a){
    if (serial_arduino_fd < 0) return;
    ssize_t w = write(serial_arduino_fd, cmd_a, strlen(cmd_a));
    if (w < 0) {
        fprintf(stderr, "[Jetson] Error sending string to Arduino: %s\n", strerror(errno));
    } else {
        pthread_mutex_lock(&print_mutex);
        //printf("[Jetson â†’ Arduino] %s", cmd_a);
        //fflush(stdout);
        pthread_mutex_unlock(&print_mutex);
    }
}

//Opening and configuration of Serial for ESP32
int open_serial_esp(const char* port_name) {
    int fd = open(port_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "[Jetson] Error opening serial %s: %s\n", port_name, strerror(errno));
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        fprintf(stderr, "[Jetson] tcgetattr failed for %s: %s\n", port_name, strerror(errno));
        close(fd);
        return -1;
    }

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "[Jetson] tcsetattr failed for %s: %s\n", port_name, strerror(errno));
        close(fd);
        return -1;
    }

    printf("[Jetson] %s port successfully initialized\n", port_name);
    return fd;
}


//Opening and configuration of Serial for Arduino
void open_serial_arduino(void){
    serial_arduino_fd = open(ARDUINO_SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_arduino_fd < 0) {
        fprintf(stderr, "[Jetson] Error opening Arduino serial: %s\n", strerror(errno));
        return;
    }

    struct termios tty;
    if (tcgetattr(serial_arduino_fd, &tty) != 0) {
        fprintf(stderr, "[Jetson] tcgetattr failed for Arduino: %s\n", strerror(errno));
        close(serial_arduino_fd);
        return;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(serial_arduino_fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "[Jetson] tcsetattr failed for Arduino: %s\n", strerror(errno));
        close(serial_arduino_fd);
        return;
    }

    pthread_t read_thread;
    pthread_create(&read_thread, NULL, serial_read_arduino_thread, NULL);

    printf("[Jetson] Arduino port successfully initialized!\n");
}
#ifndef MAIN_H
#define MAIN_H
#include <pthread.h>

#define BUF_SIZE 1024


extern pthread_mutex_t print_mutex;

void send_command(const char* cmd);
void send_arduino(float vx, float vy, float vz);
void send_string_arduino(const char* cmd_a);

#endif
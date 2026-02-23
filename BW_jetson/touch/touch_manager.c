#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "touch_manager.h"

extern pthread_mutex_t print_mutex;

// Variabili statiche per mantenere stato tra i cicli
static char last_valid_msg[32] = "";
static bool has_valid_msg = false;
static struct timespec last_valid_time;

#define TOUCH_CLEAR_MS 500

static long elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000L +
           (end.tv_nsec - start.tv_nsec) / 1000000L;
}

void touch_manager(const char *buf) {
    const char *valid_msgs[] = {"hit", "caress"};
    int num_valid_msgs = 2;

    int valid_msg_arrived = 0;

    if (buf != NULL) {
        char tmp[1024];
        strncpy(tmp, buf, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';

        char *line = strtok(tmp, "\n");
        while (line != NULL) {
            int is_valid = 0;
            for (int i = 0; i < num_valid_msgs; i++) {
                if (strstr(line, valid_msgs[i]) != NULL) {
                    is_valid = 1;
                    valid_msg_arrived = 1;

                    strncpy(last_valid_msg, valid_msgs[i], sizeof(last_valid_msg) - 1);
                    last_valid_msg[sizeof(last_valid_msg) - 1] = '\0';
                    has_valid_msg = true;
                    clock_gettime(CLOCK_MONOTONIC, &last_valid_time);

                    FILE *f = fopen(TOUCH_FILE, "w");
                    if (f) {
                        fprintf(f, "%s\n", valid_msgs[i]);
                        fflush(f);
                        fclose(f);
                    }

                    pthread_mutex_lock(&print_mutex);
                    //printf("[ESP] Valid message: %s\n", line);
                    fflush(stdout);
                    pthread_mutex_unlock(&print_mutex);

                    break;
                }
            }

            if (!is_valid) {
                pthread_mutex_lock(&print_mutex);
                //printf("[ESP] Ignored message: %s\n", line);
                fflush(stdout);
                pthread_mutex_unlock(&print_mutex);
            }

            line = strtok(NULL, "\n");
        }
    }

    // Controllo di svuotamento: lascia il messaggio per un breve tempo
    if (!valid_msg_arrived && has_valid_msg) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        if (elapsed_ms(last_valid_time, now) >= TOUCH_CLEAR_MS) {
            FILE *f = fopen(TOUCH_FILE, "w");
            if (f) fclose(f);

            pthread_mutex_lock(&print_mutex);
            //printf("[ESP] Cleared file (timeout)\n");
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);

            last_valid_msg[0] = '\0';
            has_valid_msg = false;
        }
    }
}

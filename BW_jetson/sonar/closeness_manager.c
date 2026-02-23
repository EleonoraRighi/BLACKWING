#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "closeness_manager.h"
#include "serial.h"

extern pthread_mutex_t print_mutex;

// Nomi sensori
static const char *sonar_names[NUM_SONAR] = {"FL","FCL","FC","FCR","FR","BL","B"};

// Buffer e stati
static float sonar_history[NUM_SONAR][HISTORY_SIZE];
static int history_index[NUM_SONAR];
static int values_count[NUM_SONAR];
static int collision_state[NUM_SONAR];


static FILE *video_fp = NULL;

// --- Inizializzazione ---
void init_sonar() {
    for (int i = 0; i < NUM_SONAR; i++) {
        for (int j = 0; j < HISTORY_SIZE; j++)
            sonar_history[i][j] = 70.0;
        history_index[i] = 0;
        values_count[i] = 0;
        collision_state[i] = 0;
    }

    video_fp = fopen( VIDEO_FILE, "r");
    if (!video_fp) {
        perror(VIDEO_FILE);
    }
}

// --- Aggiorna valori dai messaggi ESP_s ---
void update_sonar_data(const char *buf) {
    if (!buf || strlen(buf) == 0) return;

    char tmp[1024];
    strncpy(tmp, buf, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';

    char *line = strtok(tmp, "\n");
    while (line != NULL) {
        char buf_name[16];
        float val;

        if (sscanf(line, "[ESP_s] SONAR %15[^:]: %f", buf_name, &val) == 2) {
            int sensor_idx;
            for (sensor_idx = 0; sensor_idx < NUM_SONAR; sensor_idx++)
                if (strcmp(buf_name, sonar_names[sensor_idx]) == 0) break;

            if (sensor_idx < NUM_SONAR) {
                sonar_history[sensor_idx][history_index[sensor_idx] % HISTORY_SIZE] = val;
                history_index[sensor_idx] = (history_index[sensor_idx]+1) % HISTORY_SIZE;
                if (values_count[sensor_idx] < HISTORY_SIZE) values_count[sensor_idx]++;
            }
        }

        line = strtok(NULL, "\n");
    }
}

// --- Controllo frontali decrescenti ---
// Restituisce 1 se almeno un frontale ha gli ultimi HISTORY_SIZE valori decrescenti
int check_approaching() {
    int frontals[] = {1,2,3}; // FCL, FC, FCR
    for (int i = 0; i < 3; i++) {
        int idx = frontals[i];
        if (values_count[idx] < HISTORY_SIZE) continue; // servono almeno HISTORY_SIZE valori

        int decreasing = 1;
        for (int j = HISTORY_SIZE - 1; j > 0; j--) {
            int prev = (history_index[idx]-j-1+HISTORY_SIZE)%HISTORY_SIZE;
            int curr = (history_index[idx]-j+HISTORY_SIZE)%HISTORY_SIZE;
            if (!(sonar_history[idx][prev] > sonar_history[idx][curr])) {
                decreasing = 0;
                break;
            }
        }
        if (decreasing) {
            //printf("Approach sequence %s:", sonar_names[idx]);
            for (int j = HISTORY_SIZE - 1; j >= 0; j--) {
                int hidx = (history_index[idx]-j-1+HISTORY_SIZE)%HISTORY_SIZE;
                //printf(" %.1f", sonar_history[idx][hidx]);
            }
            //printf("\n");
            return 1;
        }
    }
    return 0;
}

// --- Controllo collision risk ---
void check_collision_risk() {
    int others[] = {0,4,5,6}; // FL, FR, BL, B
    for (int i = 0; i < 4; i++) {
        int idx = others[i];
        if (values_count[idx] == 0) continue;

        int last_idx = (history_index[idx]-1+HISTORY_SIZE)%HISTORY_SIZE;
        if (sonar_history[idx][last_idx] < APPROACH_THRESHOLD) {
            if (collision_state[idx] == 0) {
                printf("\033[1;31mCollision risk detected by sonar %s\033[0m\n",
                       sonar_names[idx]); // rosso
                collision_state[idx] = 1;
                send_string_arduino("START\n");
                send_arduino(0,0,0);
                send_string_arduino("STOP\n");
            }
        } else {
            collision_state[idx] = 0;
        }
    }
}

// --- Funzione principale ---
void closeness_manager(const char *buf) {
    update_sonar_data(buf);

    pthread_mutex_lock(&print_mutex);

    // --- Stampa frontali attivi in modo schematico ---
    //printf("Frontals history:\n");
    int frontals[] = {1,2,3}; // FCL, FC, FCR
    for (int i = 0; i < 3; i++) {
        int idx = frontals[i];
        if (values_count[idx] == 0) continue;

        //printf("%s:", sonar_names[idx]);
        for (int j = 0; j < values_count[idx]; j++) {
            int hidx = (history_index[idx]-values_count[idx]+j+HISTORY_SIZE)%HISTORY_SIZE;
            //printf(" %.1f", sonar_history[idx][hidx]);
        }
        //printf("\n");
    }

    // --- Stampa altri sonar se hanno valori ---
    for (int i = 0; i < NUM_SONAR; i++) {
        if (i==1 || i==2 || i==3) continue; // frontali giÃ  stampati
        if (values_count[i]==0) continue;

        int last_idx = (history_index[i]-1+HISTORY_SIZE)%HISTORY_SIZE;
        //printf("Sonar %s latest=%.1f values_count=%d\n", sonar_names[i],sonar_history[i][last_idx], values_count[i]);
    }

    // --- Controllo frontali ---

if (check_approaching()) {
    //printf("\033[1;33mApproaching detected!\033[0m\n"); // giallo

    if (video_fp) {
        rewind(video_fp);   // torna a inizio file
        char word[64];
        int count = 0;
        int found_person = 0;

        while (fscanf(video_fp, "%63s", word) == 1 && count < 3) {
            if (strstr(word, "person") != NULL) {
                found_person = 1;
                break;
            }
            count++;
        }

        // evita "sticky person": svuota il file dopo la lettura
        freopen(VIDEO_FILE, "w", video_fp);
        freopen(VIDEO_FILE, "r", video_fp);

        if (found_person) {
            FILE *cd_fp = fopen(CLOSENESS_FILE, "w");
            if (cd_fp) {
                fputs("closeness", cd_fp);
                fclose(cd_fp);
            } else {
                perror(CLOSENESS_FILE);
            }

            sleep(3);

            cd_fp = fopen(CLOSENESS_FILE, "w");
            if (cd_fp) {
                fclose(cd_fp); // truncate file to remove the word
            } else {
                perror(CLOSENESS_FILE);
            }
        }
    }
}

check_collision_risk();

pthread_mutex_unlock(&print_mutex);
}
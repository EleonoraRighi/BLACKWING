#include "input_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>


#define NUM_FILES 4
enum {
    FILE_AUDIO = 0,
    FILE_CLOSENESS = 1,
    FILE_LIKEDISLIKE = 2,
    FILE_TOUCH = 0//4
};

const char *file_paths[NUM_FILES] = {
    //"../touch/touch_detection.txt",
    //"../audio/audio_detection.txt",
    //"../sonar/closeness_detection.txt",
    //"../vision/likedislike_detection.txt"
    
};

typedef void (*signal_callback_t)(const char *source, const char *signal);

// --- helper: ritorna 1 se la stringa è vuota o contiene solo spazi ---
static int is_empty_or_spaces(const char *s) {
    if (!s) return 1;
    while (*s) {
        if (*s != ' ' && *s != '\t') return 0;
        s++;
    }
    return 1;
}

static long long now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000LL + (long long)ts.tv_nsec / 1000LL;
}

static void clear_signal_file(const char *path) {
    FILE *f = fopen(path, "w");
    if (f) fclose(f);
}

void *input_thread(void *arg) {
    signal_callback_t callback = (signal_callback_t)arg;
    char buffer[128];
    char last_like_dislike[128] = {0};
    long long last_like_dislike_sent_us = 0;

    // Usa /dev/tty in non-blocking per non alterare lo stdin globale
    int tty_fd = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    if (tty_fd < 0) {
        tty_fd = STDIN_FILENO; // fallback
    }

    printf("Input loop avviato: terminale + %d file\n", NUM_FILES);

    while (1) {
        bool callback_fired = false;

        // --- Terminale ---
        ssize_t n = read(tty_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            buffer[strcspn(buffer, "\n")] = 0;
            if (!is_empty_or_spaces(buffer) && callback) {
                callback("terminal", buffer);
                callback_fired = true;
            }
        }

        // --- Tutti i file ---
        for (int i = 0; i < NUM_FILES; i++) {
            FILE *f = fopen(file_paths[i], "r");
            if (f) {
                if (fgets(buffer, sizeof(buffer), f) != NULL) {
                    buffer[strcspn(buffer, "\n")] = 0; // rimuove \n
                    if (!is_empty_or_spaces(buffer) && callback) {
                        char source[32];
                        snprintf(source, sizeof(source), "file%d", i+1);
                        if (i == FILE_LIKEDISLIKE) {
                            long long now = now_us();
                            int changed = strcmp(buffer, last_like_dislike) != 0;
                            int interval_ok = (now - last_like_dislike_sent_us) >= LIKE_DISLIKE_MIN_INTERVAL_US;
                            if (changed || interval_ok) {
                                callback(source, buffer);
                                callback_fired = true;
                                if (changed) {
                                    snprintf(last_like_dislike, sizeof(last_like_dislike), "%s", buffer);
                                }
                                last_like_dislike_sent_us = now;
                            }
                        } else {
                            callback(source, buffer); // callback solo se non vuoto
                            callback_fired = true;
                            // Consume one-shot signals to avoid retriggering at each poll.
                            clear_signal_file(file_paths[i]);
                        }
                    }
                }
                fclose(f);
            }
        }

        if (callback_fired) {
            usleep(COOLDOWN_US);
        }
        usleep(POLL_DELAY_US);
    }

    return NULL;
}

void input_initialize(signal_callback_t callback) {
    pthread_t tid;
    if (pthread_create(&tid, NULL, input_thread, (void *)callback) != 0) {
        perror("pthread_create");
        exit(1);
    }
    pthread_detach(tid);
}

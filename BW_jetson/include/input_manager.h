#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H


#define POLL_DELAY_US 100000
#define COOLDOWN_US 500000
#define LIKE_DISLIKE_MIN_INTERVAL_US 30000000

typedef void (*signal_callback_t)(const char *source, const char *signal);

// funzione per attivare tutti i task che raccolgono i segnali
void input_initialize(signal_callback_t callback);

#endif

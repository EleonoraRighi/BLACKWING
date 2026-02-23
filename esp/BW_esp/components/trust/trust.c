#include "trust.h"
#include "main.h"
#include "motor_declarations.h"
#include "sequences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

static const char *TAG = "trust";
static char previous_action[16];

void acceptance(void){
    uart_send_msg("\033[35mAcceptance triggered\033[0m\n");
}
void trust(void){
    uart_send_msg("\033[35mTrust triggered\033[0m\n");
}
void admiration(void){
    uart_send_msg("\033[35mAdmiration triggered!\033[0m\n");
}
#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t sadness_init(void);
void sadness_modify(float action, const char *action_name);

void pensiveness(void);
void sadness(void);
void grief(void);

//other public functions
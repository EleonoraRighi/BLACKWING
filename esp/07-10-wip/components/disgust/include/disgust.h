#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t disgust_init(void);
void disgust_modify(float action, const char *action_name);

void boredom(void);
void disgust(void);
void loathing(void);
//other public functions
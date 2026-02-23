#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t surprise_init(void);
void surprise_modify(float action, const char *action_name);

void distraction(void);
void surprise(void);    
void amazement(void);

//other public functions
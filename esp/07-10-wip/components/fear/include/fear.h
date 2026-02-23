#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t fear_init(void);
void fear_modify(float action, const char *action_name);

void apprehension(void);
void fear(void);
void terror(void);

//other public functions
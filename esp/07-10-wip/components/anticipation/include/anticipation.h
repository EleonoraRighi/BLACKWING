#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t anticipation_init(void);
void anticipation_modify(float action, const char *action_name);

void vigilance(void);
void anticipation(void);
void interest(void);    

//other public functions
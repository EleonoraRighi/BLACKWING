#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t trust_init(void);
void trust_modify(float action, const char *action_name);

void acceptance(void);
void trust(void);
void admiration(void);


//other public functions
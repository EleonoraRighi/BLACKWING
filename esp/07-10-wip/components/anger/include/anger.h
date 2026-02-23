#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t anger_init(void);
void anger_modify(float action, const char *action_name);
void annoyance(void);
void anger(void);
void rage(void);

void anger_test(void);
//other public functions
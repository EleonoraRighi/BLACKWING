#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t idle_init(void);
void idle(void);

//other public functions
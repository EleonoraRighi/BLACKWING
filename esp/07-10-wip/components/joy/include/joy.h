#pragma once
#include "esp_err.h"

/* API pubblica del componente */
esp_err_t joy_init(void);
void joy_modify(float action, const char *action_name);

void serenity(void);
void joy(void); 
void ecstasy(void);

//other public functions
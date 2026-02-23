#pragma once
#include "esp_err.h"


//variabili esterne
extern float curiosity;
/* API pubblica del componente */
esp_err_t curiosity_init(void);
void curiosity_increase(float value);
void curiosity_decrease(float value) ;


//other public functions
#pragma once   
#include <stdio.h>
#include "iot_servo.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"
#include "esp_timer.h"  // serve per misurare tempo in microsecondi


// OpenClose_Wing  servo configuration
#define SERVO_CH0_PIN 32
#define MAX_ANGLE_1 100
#define MIN_ANGLE_1 5
#define STARTING_ANGLE 140




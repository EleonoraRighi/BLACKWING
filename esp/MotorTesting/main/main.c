#include <stdio.h>
#include "motor_declaration.h"
#include "speed1.h"
#include "speed2.h"
#include "fear_test.h"
#include "angle_test.h"
#include "iot_servo.h"      // API del componente servo (servo_config_t, iot_servo_*)
#include "driver/ledc.h"    // LEDC/LEDC_CHANNEL_x, LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, ecc.
#include "esp_err.h"        // esp_err_t (opzionale, ma utile)
#include "esp_log.h"        // log (opzionale)


void app_main(void)
{
//    printf("Starting speed1...\n");
//    app_main_motor1();
//    printf("Starting speed2...\n");
//    app_main_motor2();

/*   motor_init();
    printf("Starting angle test...\n");
    for(int i=0; i<180; i++){ {
        
        modify_angle();

*/ 



    fear_init();
    vTaskDelay(pdMS_TO_TICKS(1000)); // lascia partire timer
    //fear_run_animation();

}


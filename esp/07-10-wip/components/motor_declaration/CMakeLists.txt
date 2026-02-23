idf_component_register( SRCS "motor_declaration.c" "servo_manager.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver freertos main)  
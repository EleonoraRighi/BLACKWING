#ifndef SERIAL_H
#define SERIAL_H

#define ESP1_SERIAL_PORT "/dev/esp1"      // ESP32
#define ESP2_SERIAL_PORT "/dev/esp2"      //ESP32 for sonars
#define ARDUINO_SERIAL_PORT   "/dev/ttyACM0" // Arduino

// SERIALS
extern int serial_esp1_fd;           //ESP32
extern int serial_esp2_fd;           //ESP32 for sonars
extern int serial_arduino_fd;       //Arduino

typedef struct {
    int fd;
    int esp_id;
} serial_args_t;


void* serial_read_thread(void* arg);                                  //Thread for serial communication with ESP32
void* serial_read_arduino_thread(void* arg);                          //Thread for serial communication with Arduino

void send_command(const char* cmd);                                   //To send commands to ESP32 (String)
void send_arduino(float speed_x, float speed_y, float angular_speed); //To send commands to Arduino (Json)
void send_string_arduino(const char* cmd_a);                          //To send commands to Arduino (String)

int open_serial_esp(const char* port_name);                                           //Opening and configuration of Serial for ESP32
void open_serial_arduino(void);                                       //Opening and configuration of Serial for Arduino

#endif
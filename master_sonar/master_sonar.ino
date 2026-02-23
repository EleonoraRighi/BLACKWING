#include "NewPing.h"
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

//SONAR
#define SONAR_NUM     7 // Number of sensors.
#define MAX_DISTANCE 80 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define PI 3.1415926535897932384626433
#define RAD_EXT 0.977384
#define RAD_INT 1.25664
#define MAX_DIST_INT 15 //10 
#define MAX_DIST_EXT 20 //15 
#define MAX_DIST_FRONT 25 //20

//FOR GAME 4
#define MIN_DIST_GO 30 //15 30
#define MAX_DIST_GO 90 //60 90
#define MAX_DIST_GO_LAT 60 //45 60
#define MIN_DIST_GO_LAT 30 //15 30
#define MAX_DIST_GO_BACK 30 //15 30
#define MIN_DIST_GO_BACK 10 //5 10

int cm[SONAR_NUM] = {0,0,0,0,0,0,0};  // Where the ping distances are stored.
float dist[SONAR_NUM]  = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};  // Where the ping distances are stored.
struct prevDist{
  int num_wrong;
  float prev_val;
};
prevDist prev_dist[SONAR_NUM];
struct DistPacket {
  uint8_t header;      // Start byte (for sync)
  int values[8];    
  uint8_t checksum;    // Simple XOR checksum
};
DistPacket packet;
uint8_t broadcastAddress[] = {0xCC, 0xDB, 0xA7, 0x93, 0xCF, 0xF0};
typedef struct struct_message {
    int id; // must be unique for each sender board
    int values[8];
} struct_message;
struct_message packetNOW;
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  /*Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");*/
}

NewPing sonar[SONAR_NUM]{
  NewPing(4, 13, MAX_DISTANCE), //Front_Left    // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(14, 18, MAX_DISTANCE), //Front_CenterLeft
  NewPing(19, 21, MAX_DISTANCE), //Front_Center
  NewPing(22, 23, MAX_DISTANCE), //Front_CenterRight
  NewPing(25, 26, MAX_DISTANCE), //Front_Right
  NewPing(27, 32, MAX_DISTANCE), //Back_Left
  NewPing(33, 34, MAX_DISTANCE)  //Back_Right
};

int state;
int prev_state = 10;
int state_2;
int prev_state_2 = 10;
int i1 = 0;
int i2 = 0;
int i3 = 0;
int tilt[7][SONAR_NUM] = {0};  
int retro = 1;
long prev_stallT = 0;
long prev_turnT = 0;
int interval_stallT = 3000;
int interval_turnT = 1000; // FROBOTTO -> 2000;
int prev_dir = 4;
bool stall = false;
typedef struct sonar_fuzzy {
  float near;
  float slow_near;
  float range;
  float far;
} sonar_fuzzy;

sonar_fuzzy S_L = { 0.0, 0.0, 0.0, 0.0};
sonar_fuzzy S_CL = { 0.0, 0.0, 0.0, 0.0};
sonar_fuzzy S_C = { 0.0, 0.0, 0.0, 0.0};
sonar_fuzzy S_CR = { 0.0, 0.0, 0.0, 0.0};
sonar_fuzzy S_R = { 0.0, 0.0, 0.0, 0.0};

int s_follow = 4;
int count_stop = 0;

#define FILTER_SIZE 4
int buffer_L[FILTER_SIZE];
int i_L = 0;
int buffer_CL[FILTER_SIZE];
int i_CL = 0;
int buffer_C[FILTER_SIZE];
int i_C = 0;
int buffer_CR[FILTER_SIZE];
int i_CR = 0;
int buffer_R[FILTER_SIZE];
int i_R = 0;
int buffer_BL[FILTER_SIZE];
int i_BL = 0;
int buffer_BR[FILTER_SIZE];
int i_BR = 0;

void init_prevDist(){
  for(int i = 0; i< SONAR_NUM; i++){
    prev_dist[i].num_wrong = 30;
    prev_dist[i].prev_val = 0.0;
  }
}

int movingAverage(int newValue, int num) {
  long sum = 0;
  int count = 0;
  //non tengo conto dei valori 0, out of range

  switch(num){
    case 0:
      buffer_L[i_L] = newValue;
      i_L = (i_L + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_L[i] > 0){
          sum += buffer_L[i];
          count++;
        }
      }
      break;
    case 1:
      buffer_CL[i_CL] = newValue;
      i_CL = (i_CL + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_CL[i] > 0){
          sum += buffer_CL[i];
          count++;
        }
      }
      break;
    case 2:
      buffer_C[i_C] = newValue;
      i_C = (i_C + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_C[i] > 0){
          sum += buffer_C[i];
          count++;
        }
      } 
      break;
    case 3:
      buffer_CR[i_CR] = newValue;
      i_CR = (i_CR + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_CR[i] > 0){
          sum += buffer_CR[i];
          count++;
        }
      }
      break;
    case 4:
      buffer_R[i_R] = newValue;
      i_R = (i_R + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_R[i] > 0){
          sum += buffer_R[i];
          count++;
        }
      } 
      break;
    case 5:
      buffer_BL[i_BL] = newValue;
      i_BL = (i_BL + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_BL[i] > 0){
          sum += buffer_BL[i];
          count++;
        }
      }
      break;
    case 6:
      buffer_BR[i_BR] = newValue;
      i_BR = (i_BR + 1) % FILTER_SIZE;
      for (int i = 0; i < FILTER_SIZE; i++){
        if(buffer_BR[i] > 0){
          sum += buffer_BR[i];
          count++;
        }
      }
      break;
  }
  
  if(count > 0)
    return sum / count;
  
  return 70;
}


//DICHIARAZIONE FUNZIONI
int stato(int[]);
void cerca(int[] , NewPing[]);

void sendPacket(int state, int cm[]) {
  packet.header = 0xAA;  // Unique start marker
  packet.values[0] = state;
  for(int i=0; i < 7; i++){
    packet.values[i+1] = cm[i];
  }
  
  // Compute checksum
  uint8_t *data = (uint8_t*)&packet.values;
  packet.checksum = 0;
  for (int i = 0; i < sizeof(packet.values); i++)
    packet.checksum ^= data[i];
  // Send raw bytes
  Serial.print("VALUE 1: ");
  Serial.println(packet.values[1]);
  Serial.print("DIRECTION STATE: ");
  Serial.println(packet.values[0]);
  Serial.print("HEADER Value: ");
  Serial.println(packet.header, HEX);
  Serial.println("-------------------");
  Serial2.write((uint8_t*)&packet, sizeof(packet));
}

//************************************************************************************************************************************ SETUP

void init_espNOW(){
   // Init ESP-NOW
  uint8_t channel = 6; 
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void setup() {
  Serial.begin(115200); //comunicazione col PC
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // Initialize UART2 (TX: GPIO 17, RX: GPIO 16)
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  init_espNOW();
  init_prevDist();
  //Serial2.print((int)4);  // Send distance data to the other ESP32
  //Serial2.print('\n');  // Newline to mark end of data
}


//******************************************************************************************************************************************** LOOP

char game = '0';

void loop() {

  if(Serial2.available()){
    game = (Serial2.readStringUntil('\n'))[0];  // Read the received data
    Serial.print("MSG received : ");
    Serial.println(game);
  }

  cerca(&cm[0], &sonar[0]);   //cerca ostacoli con i sonar
  
  if(game == '0'){
    state = stato(&cm[0]);     //imposta lo stato 
    
    Serial.print("Direction: ");
    Serial.println(state);
    //PREV UPDATE FOR DIAGNOSTIC
    if(prev_state != state){  
      Serial2.print(state);  // Send distance data to the other ESP32
      Serial2.print('\n');  // Newline to mark end of data
      prev_state = state;
    }
    //sendPacket(state, &cm[0]);
    packetNOW.id = 1;
    
    packetNOW.values[0] = state;
    int i = 1;
    while(i<8){
      if(cm[i-1] == 0)
        packetNOW.values[i] = 90;
      else
        packetNOW.values[i] = cm[i-1];
      i++;
    }
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &packetNOW, sizeof(packetNOW));
   
    if (result == ESP_OK) {
      //Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    //prev_state = state;
  }
  else if(game == '4'){
    // sorcerer game
    state_2 = state_fuzzy(&cm[0]);
    Serial.print("Direction S.: ");
    Serial.println(state_2);
    //PREV UPDATE FOR DIAGNOSTIC
    if(prev_state_2 != state_2){  
      Serial2.print(state_2);  // Send distance data to the other ESP32
      Serial2.print('\n');  // Newline to mark end of data
      prev_state_2 = state_2;
    }
    /*sendPacket(state_2, &cm[0]);
    prev_state_2 = state_2;*/
    packetNOW.id = 1;
    
    packetNOW.values[0] = state_2;
    int i = 1;
    while(i<8){
      //if(cm[i-1] == 0)
        //packetNOW.values[i] = 90;
      //else
        packetNOW.values[i] = (int)dist[i-1];
      i++;
    }
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &packetNOW, sizeof(packetNOW));
   
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }

  }
  
  delay(50);
}

float EWMA(int cm, int prev){
  float alpha = 0.5;
  return alpha*cm + (1-alpha)*prev;
}

float fuzzy_near(float cm){
   //FROBOTTO
  if (cm <= 10.0 && cm > 0.0)
    return 1.0;
  if (cm > 10.0 && cm < 20.0)
    return (20.0 - cm) / 10.0;
  
  /*if (cm <= 10.0 && cm > 0.0)
    return 1.0;
  if (cm > 10.0 && cm < 25.0)
    return (25.0 - cm) / 15.0;*/

  return 0.0;
}

float fuzzy_slow_near(float cm){
   //FROBOTTO
  if (cm > 10.0 && cm < 20.0)
    return (cm - 10.0) / 10.0;
  if (cm >= 20.0 && cm <= 30.0)
    return 1.0;
  if(cm > 30.0 && cm <= 40.0)
    return (40.0 - cm) / 10.0;
  
  /*if (cm <= 10.0 && cm > 0.0)
    return 1.0;
  if (cm > 10.0 && cm < 25.0)
    return (25.0 - cm) / 15.0;*/

  return 0.0;
}

float fuzzy_inRange(float cm){
   //FROBOTTO
  if (cm > 30.0 && cm < 40.0)
    return (cm - 30.0) / 10.0;
  if (cm >= 40.0 && cm < 55.0)
    return 1.0;
  if (cm >= 55.0 && cm <= 65.0)
    return (65.0 - cm) / 10.0;

  /*if (cm > 15.0 && cm < 30.0)
    return (cm - 15.0) / 15.0;
  if (cm >= 30.0 && cm <= 45.0)
    return (45.0 - cm) / 15.0;*/
  return 0.0;
}

float fuzzy_far(float cm){
  //FROBOTTO
  if (cm >= 70.0)
    return 1.0;
  if (cm > 55.0 && cm < 70.0)
    return (cm - 55.0) / 15.0;
  
  /*if (cm >= 60.0)
    return 1.0;
  if (cm > 45.0 && cm < 60.0)
    return (cm - 45.0) / 15.0;*/
  return 0.0;
}

float find_oor_dist_ext(prevDist* prev, int next){
  if((*prev).prev_val == 0){
    if(next == 0)
      return 70.0;
    else
      return (sqrt(2*pow(next, 2) + 162)/2);
  }
  else{
    if(next == 0){
      if((*prev).prev_val > 50)
        return 70.0;
      else{
        (*prev).num_wrong++;
        if((*prev).num_wrong > 10){
          (*prev).num_wrong = 0;
          return 70.0;
        }
        return (*prev).prev_val;
      }
    }
    else{
      (*prev).num_wrong = 0;
      return (((sqrt(2*pow(next, 2) + 162)/2) + (*prev).prev_val) / 2);
    }
  }
}

float find_oor_dist_int(prevDist* prev, int n_p, int n_s){
  if((*prev).prev_val == 0 && n_p == 0 && n_s == 0)
    return 70.0;
  
  if((*prev).prev_val == 0){
    if(n_s == 0)
      return (sqrt(2*pow(n_p, 2) + 162)/2);
    
    if(n_p == 0)
      return (sqrt(2*pow(n_s, 2) + 162)/2);
    
    if(n_p <= n_s)
      return (((2*(sqrt(2*pow(n_p, 2) + 162)/2)) + (sqrt(2*pow(n_s, 2) + 162)/2))/3);
    return (((2*(sqrt(2*pow(n_s, 2) + 162)/2)) + (sqrt(2*pow(n_p, 2) + 162)/2))/3);
  }
  else{
    if(n_s == 0 && n_p == 0){
      if((*prev).prev_val > 50)
        return 70.0;
      else{
        (*prev).num_wrong++;
        if((*prev).num_wrong > 10){
          (*prev).num_wrong = 0;
          return 70.0;
        }
        return (*prev).prev_val;
      }
    }
    
    if(n_s == 0){
      (*prev).num_wrong = 0;
      return (((sqrt(2*pow(n_p, 2) + 162)/2) + (*prev).prev_val) / 2);
    }

    if(n_p == 0){
      (*prev).num_wrong = 0;
      return (((sqrt(2*pow(n_s, 2) + 162)/2) + (*prev).prev_val) / 2);
    }

    if(n_p <= n_s)
      return (((((2*(sqrt(2*pow(n_p, 2) + 162)/2)) + (sqrt(2*pow(n_s, 2) + 162)/2))/3) + (*prev).prev_val)/2);
    return (((((2*(sqrt(2*pow(n_s, 2) + 162)/2)) + (sqrt(2*pow(n_p, 2) + 162)/2))/3) + (*prev).prev_val)/2);
    
  }
}

//*************************************************************************************************************************************** FUNZIONI

int state_fuzzy(int cm[]){

  /*dist[0] = EWMA(cm[0], dist[0]);
  dist[1] = EWMA(cm[1], dist[1]);
  dist[2] = EWMA(cm[2], dist[2]);
  dist[3] = EWMA(cm[3], dist[3]);
  dist[4] = EWMA(cm[4], dist[4]);
  dist[5] = EWMA(cm[5], dist[5]);
  dist[6] = EWMA(cm[6], dist[6]);*/
  dist[0] = cm[0];
  dist[1] = cm[1];
  dist[2] = cm[2];
  dist[3] = cm[3];
  dist[4] = cm[4];
  dist[5] = cm[5];
  dist[6] = cm[6];

  /*for(int i = 0; i < (SONAR_NUM-2); i++){
    /*if(abs(prev_dist[i].prev_val - cm[i]) > 40){
      if(prev_dist[i].num_wrong >= 30){
        //Aggiorno dist
        dist[i] = cm[i];
        prev_dist[i].prev_val = dist[i];
        prev_dist[i].num_wrong = 0;
      }
      else{
        dist[i] = prev_dist[i].prev_val;
        prev_dist[i].num_wrong++;
      }
    }
    if(cm[i] == 0){
      if(i == 0){
        dist[i] = find_oor_dist_ext(&prev_dist[i], cm[i+1]);
      }
      else if(i == 4){
        dist[i] = find_oor_dist_ext(&prev_dist[i], cm[i-1]);
      }
      else{
        dist[i] = find_oor_dist_int(&prev_dist[i], cm[i-1], cm[i+1]);
      }
    }
    else{
      dist[i] = cm[i];
      prev_dist[i].prev_val = dist[i];
      prev_dist[i].num_wrong = 0;
    }

    Serial.println("---------------");
    Serial.print(i);
    Serial.print("   |   CM : ");
    Serial.print(cm[i]);
    Serial.print("   |   DIST : ");
    Serial.println(dist[i]);

  }*/



  float maxVal = 0.0;
  int next_state = s_follow;


  if(millis() - prev_turnT < interval_turnT){
    s_follow = 5;
  }
  else{
    prev_turnT = 0;

    S_L.near = fuzzy_near(dist[0]);
    S_CL.near = fuzzy_near(dist[1]);
    S_C.near = fuzzy_near(dist[2]);
    S_CR.near = fuzzy_near(dist[3]);
    S_R.near = fuzzy_near(dist[4]);

    S_L.slow_near = fuzzy_slow_near(dist[0]);
    S_CL.slow_near = fuzzy_slow_near(dist[1]);
    S_C.slow_near = fuzzy_slow_near(dist[2]);
    S_CR.slow_near = fuzzy_slow_near(dist[3]);
    S_R.slow_near = fuzzy_slow_near(dist[4]);

    S_L.range = fuzzy_inRange(dist[0]);
    S_CL.range = fuzzy_inRange(dist[1]);
    S_C.range = fuzzy_inRange(dist[2]);
    S_CR.range = fuzzy_inRange(dist[3]);
    S_R.range = fuzzy_inRange(dist[4]);

    S_L.far = fuzzy_far(dist[0]);
    S_CL.far = fuzzy_far(dist[1]);
    S_C.far = fuzzy_far(dist[2]);
    S_CR.far = fuzzy_far(dist[3]);
    S_R.far = fuzzy_far(dist[4]);

    float for_fast = S_C.range;
    float for_slow = S_C.slow_near;
    //float for_L_slow = max(S_CL.range, min(S_C.range, S_L.range));
    float for_L_slow = min(S_CL.range, min(S_CL.slow_near, min(S_L.slow_near, S_L.range)));
    //float for_R_slow = max(S_CR.range, min(S_C.range, S_R.range));
    float for_R_slow = min(S_CR.range, min(S_CR.slow_near, min(S_R.slow_near, S_R.range)));
    float left_fast = 0.3*S_CL.range + 0.7*S_L.range;
    float right_fast = 0.3*S_CR.range + 0.7*S_R.range;
    float stop = max(S_L.near, max(S_CL.near, max(S_C.near, max(S_CR.near, S_R.near))));

    /*float stato = max(for_fast, max(for_L_slow, max(for_R_slow, max(left_fast, max(right_fast, stop)))));
    
    if(stato != 0.0){
      //s_follow = 4;

      if(stato == for_fast)
        s_follow = 0;
      else if(stato == for_L_slow)
        s_follow = 6;
      else if(stato == for_R_slow)
        s_follow = 7;
      else if(stato == left_fast)
        s_follow = 1;
      else if(stato == right_fast)
        s_follow = 2;
      else if(stato == stop){
        count_stop++;
        if(count_stop > 10){
          s_follow = 4;
          if(!stall){
            prev_stallT = millis();
            stall = true;
          }
          count_stop = 0;
        }
      } else{
        if(s_follow == 5)
          s_follow = 4;
      }
    }
    else{
      s_follow = 0;
    }
    }
    */
    
    if (stop >= 1.0) {
      //count_stop++;
      //if (count_stop > 10) {
        maxVal = 1.0;
        next_state = 4; // STOP
        if (!stall) {
          prev_stallT = millis();
          stall = true;
        }
        //count_stop = 0;
      //}
    } 
    else {
      count_stop = 0;
      if (for_fast > maxVal)      { maxVal = for_fast; next_state = 0; }
      if (for_slow > maxVal)      { maxVal = for_slow; next_state = 8; }
      if (for_L_slow > maxVal)    { maxVal = for_L_slow; next_state = 6; }
      if (for_R_slow > maxVal)    { maxVal = for_R_slow; next_state = 7; }
      if (left_fast > maxVal)     { maxVal = left_fast; next_state = 1; }
      if (right_fast > maxVal)    { maxVal = right_fast; next_state = 2; }
      if (stop > maxVal /*&& stop < 0.8*/) {maxVal = stop; next_state = 8;}
    }
    if(maxVal == 0.0){
      next_state = 4;
    }
    /*if((next_state == 1 || next_state == 2) && (prev_state == 0 || prev_state == 6 || prev_state == 7) && count_prevState > 50){
      next_state = prev_state;
    }*/
    /*if(maxVal == 0.0){
      count_stop++;
      if (count_stop > 10) {
        maxVal = 1.0;
        next_state = 4; // STOP
        if (!stall) {
          prev_stallT = millis();
          stall = true;
        }
        count_stop = 0;
      }
    }*/
  }

  if(stall && millis()-prev_stallT < interval_stallT){
    if(next_state != 4){
      stall = false;
      prev_stallT = 0;
      Serial.println("-----------HERE--------------");
    }
  }
  else if(stall){
    next_state = 5;
    prev_turnT = millis();
    stall = false;
    prev_stallT = 0;
  }

  s_follow = next_state;
  return s_follow;


}

int state_follow(int cm[]){
  int s_follow = 0;

  dist[0] = cm[0];
  dist[1] = cm[1];
  dist[2] = cm[2];
  dist[3] = cm[3];
  dist[4] = cm[4];
  dist[5] = cm[5];
  dist[6] = cm[6];

  int mid_dist = (MAX_DIST_GO_LAT - MIN_DIST_GO_LAT) / 2;

  // STOP if too close or too far
  // GO if the distance from the obstacle is in range
  bool front = ((dist[2] > MIN_DIST_GO && dist[2] < MAX_DIST_GO)); // || ( (dist[1] > MIN_DIST_GO && dist[1] < MAX_DIST_GO) || (dist[3] > MIN_DIST_GO && dist[3] < MAX_DIST_GO)));
  bool left = ((dist[0] > MIN_DIST_GO_LAT && dist[0] < (MAX_DIST_GO_LAT)));
  bool right = ((dist[4] > MIN_DIST_GO_LAT && dist[4] < (MAX_DIST_GO_LAT)));
  bool fcl_far = (dist[1] > mid_dist && dist[1] < MAX_DIST_GO_LAT);
  bool fcl_near = (dist[1] > MIN_DIST_GO_LAT && dist[1] < mid_dist);
  bool fcr_far = (dist[3] > mid_dist && dist[3] < MAX_DIST_GO_LAT);
  bool fcr_near = (dist[3] > MIN_DIST_GO_LAT && dist[3] < mid_dist);
  /*bool fcl = (dist[1] > MIN_DIST_GO_LAT && dist[1] < MAX_DIST_GO_LAT);
  bool fcr = (dist[3] > MIN_DIST_GO_LAT && dist[3] < MAX_DIST_GO_LAT);*/
  bool back = ((dist[5] > MIN_DIST_GO_BACK && dist[5] < (MAX_DIST_GO_BACK)) || (dist[6] > MIN_DIST_GO_BACK && dist[6] < (MAX_DIST_GO_BACK)));

  //if(!left && !right){
    // Obstacle is in front of the robot (in range for guidance)
  

  if(millis() - prev_turnT < interval_turnT){
    s_follow = 5;
  }
  else{
    prev_turnT = 0;
  if((dist[0] < MIN_DIST_GO_LAT && dist[0] > 0) || (dist[2] < MIN_DIST_GO && dist[2] > 0) || (dist[4] < MIN_DIST_GO_LAT && dist[4] > 0)){
    s_follow = 4;
    if(!stall){
      prev_stallT = millis();
      stall = true;
    }
  }
  
  else if((front || left) && (fcl_near || fcl_far))
    s_follow = 6;
  else if((front || right) && (fcr_near || fcr_far))
    s_follow = 7;
  else if(front){
    s_follow = 0;
  }
  else if((left) && !right){
    // Obstacle is at the left of the robot (in range for guidance)
    s_follow = 1;
  }
  else if((right) && !left){
    // Obstacle is at the right of the robot (in range for guidance)
    s_follow = 2;
  }
  else if(front && (left || right)){
    
    if(fcl_near || fcl_far)
      s_follow = 6;
    else if(fcr_near || fcr_far)
      s_follow = 7;
    else{
      s_follow = 0;
    }
    /*if((dist[2] < dist[0]) && (dist[2] < dist[4]))
      s_follow = 0;
    else if((dist[0] > MIN_DIST_GO_LAT) && (dist[0] < dist[2]) && (dist[0] < dist[4]))
      s_follow = 1;
    else if((dist[4] > MIN_DIST_GO_LAT) && ((dist[4] < dist[0]) && (dist[4] < dist[2])))
      s_follow = 2;*/
  }
  /*else if(back){
    s_follow = 2;
    prev_backT = millis();
  }*/
  else if(!front && !right && ! left){
    // Obstacle is not in range (STOP)
    if(fcl_far || fcr_far)
      s_follow = 0;
    else if(fcl_near)
      s_follow = 6;
    else if(fcr_near)
      s_follow = 7;
    else{
      count_stop++;
      if(count_stop > 10){
        s_follow = 4;
        count_stop = 0;
      }
    }
    
  }
  }
  
  if(stall && millis()-prev_stallT < interval_stallT){
    if(s_follow != 4){
      stall = false;
      prev_stallT = 0;
    }
  }
  else if(stall){
    s_follow = 5;
    prev_turnT = millis();
    stall = false;
    prev_stallT = 0;
  }

  return s_follow;


}


int stato(int cm[]) {
  int stato = 0;

  dist[0] = cm[0]*cos(RAD_EXT);
  dist[1] = cm[1]*cos(RAD_INT);
  dist[2] = cm[2];
  dist[3] = cm[3]*cos(RAD_INT);
  dist[4] = cm[4]*cos(RAD_EXT);
  dist[5] = cm[5];
  dist[6] = cm[6];

  bool front = (dist[2] > 0 && dist[2] < MAX_DIST_FRONT);
  bool sx = ((dist[0] > 0 && dist[0] < MAX_DIST_EXT) || (dist[1] > 0 && dist[1] < MAX_DIST_INT));
  //bool sx = ((dist[1] > 0 && dist[1] < MAX_DIST_INT));

  bool dx = ((dist[4] > 0 && dist[4] < MAX_DIST_EXT) || (dist[3] > 0 && dist[3] < MAX_DIST_INT));
  //bool dx = ((dist[3] > 0 && dist[3] < MAX_DIST_INT));

  bool back_stop = ((dist[5] > 0 && dist[5] < MAX_DIST_FRONT) || (dist[6] > 0 && dist[6] < MAX_DIST_FRONT));
  bool back_move = ((dist[5] > 0 && dist[5] < MAX_DIST_INT) || (dist[6] > 0 && dist[6] < MAX_DIST_INT));

  if((front || (sx && dx)) && back_stop){
    //STOP
    stato = 4;
  }
  else if((front || (sx && dx))){
    //BACKWARD
    stato = 3;
  }
  else if(sx || dx){
    //TURN
    if(sx){
      //TURN RIGHT, OBSTACLE TO THE LEFT
      stato = 2;
    }
    else{
      //TURN LEFT, OBSTACLE TO THE RIGHT
      stato = 1;
    }
  }
  else if(back_stop){
    //FORWARD
    stato = 0;
  }

    
  /*if( (cm[0] || cm[1]) && (!cm[4] && !cm[3] && !cm[2])){   //ostacolo a sinistra, gira a destra
    
    stato = 2;
  }
  else if( (cm[4] || cm[3]) && (!cm[0] && !cm[1] && !cm[2])  )  //ostacolo a destra, giro a sinistra
    stato = 1;  
    
  else if ( (cm[4] || cm[3] || cm[0] || cm[1] || cm[2]) && (!cm[5] && !cm[6])) //ostacolo davanti, gira 180        
    stato = 3; 

  else if( (cm[5] || cm[6]) && (!cm[1] && !cm[2] && !cm[3]) ) //vai avanti
    stato = 0;

  else if(cm[2] && cm[3] && cm[1] && (cm[5] || cm[6])){
    if(!cm[0])
      stato = 1;
    else if(!cm[4])
      stato = 2;
    else
      stato = 4;
  }
  else
    stato = 0;
*/
  return stato;
}

void cerca(int cm[] , NewPing sonar[]) {  //calcola distanze dei sonar 

  cm[1] = movingAverage(sonar[1].ping_cm(), 1);
  cm[6] = movingAverage(sonar[6].ping_cm(), 6); 
  cm[3] = movingAverage(sonar[3].ping_cm(), 3);
  cm[2] = movingAverage(sonar[2].ping_cm(), 2);
  cm[5] = movingAverage(sonar[5].ping_cm(), 5);
  cm[0] = movingAverage(sonar[0].ping_cm(), 0); 
  cm[4] = movingAverage(sonar[4].ping_cm(), 4);  

  /*Serial.println("---------------");
  Serial.print("SONAR FL: ");
  Serial.println(cm[0]);*/
  /*Serial.print("SONAR FCL: ");
  Serial.println(cm[1]);
  Serial.print("SONAR FC: ");
  Serial.println(cm[2]);
  Serial.print("SONAR FCR: ");
  Serial.println(cm[3]);
  Serial.print("SONAR FR: ");
  Serial.println(cm[4]);
  Serial.print("SONAR BL: ");
  Serial.println(cm[5]);
  Serial.print("SONAR B: ");
  Serial.println(cm[6]); 
  Serial.println("---------------");*/
}
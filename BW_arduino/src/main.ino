#include <NewPing.h>
#include <CytronMotorDriver.h>
#include <Encoder.h>
#include <ViRHas.h>
#include <ArduinoJson.h>

// ------------------ SONARS ------------------

#define SONAR_NUM 4 
#define MAX_DISTANCE 300

float dist[SONAR_NUM];
float cm[SONAR_NUM];
float cm_prec[SONAR_NUM];
float cm_raw[SONAR_NUM];
float cm_prec1[SONAR_NUM];
float cm_prec2[SONAR_NUM];
float cm_prec3[SONAR_NUM];
float cm_prec4[SONAR_NUM];
int numActualData;

const int trigPin = 32;
const int echoPin = 30;
const int trigPin1 = 28;
const int echoPin1 = 26;
const int trigPin2 = 36;
const int echoPin2 = 34;
const int trigPin3 = 24;
const int echoPin3 = 22;

NewPing sonar[SONAR_NUM] = {
  NewPing(trigPin2, echoPin2, MAX_DISTANCE),
  NewPing(trigPin1, echoPin1, MAX_DISTANCE),
  NewPing(trigPin, echoPin, MAX_DISTANCE),
  NewPing(trigPin3, echoPin3, MAX_DISTANCE),
};

// ------------------ MOTORS ------------------

#define _EP11 2
#define _EP12 3
#define _EP31 21
#define _EP32 20
#define _EP21 19
#define _EP22 18

#define _1_1A 11
#define _1_1B 5
#define _1_2A 7
#define _1_2B 4
#define _2_1A 8
#define _2_1B 6

#define _MAX_SPEED 100
#define _MAX_ANGULAR 6.28
#define wheel_radius  3.0f
#define robot_radius  16.5f
#define encoder_ppr 1920.0f

CytronMD motor1(PWM_PWM, _2_1A, _2_1B);
CytronMD motor2(PWM_PWM, _1_1A, _1_1B);
CytronMD motor3(PWM_PWM, _1_2A, _1_2B);

Encoder ENCODER[] = {
  Encoder(_EP11, _EP12),
  Encoder(_EP21, _EP22),
  Encoder(_EP31, _EP32)
};

bool alt=true;


ViRHaS virhas = ViRHaS(motor1, motor2, motor3, ENCODER[0], ENCODER[1], ENCODER[2]);

// ------------------ JSON COMMAND STRUCT ------------------

typedef struct {
  float linear_x;
  float linear_y;
  float angular_z;
} Twist;
Twist twist_msg = {0.0, 0.0, 0.0};
// ------------------ MOVE ROBOT ------------------

void moveRobot(const Twist *twist_msg) {
  if (twist_msg->linear_x == 0.0 &&
      twist_msg->linear_y == 0.0 &&
      twist_msg->angular_z == 0.0) {

    virhas.stop();

  } else {

    virhas.run2(
      twist_msg->linear_x * _MAX_SPEED,
      twist_msg->linear_y * _MAX_SPEED,
      twist_msg->angular_z * _MAX_ANGULAR
    );

    char debug_msg_static[200];
    
    if(!alt){
      virhas.PIDLoop(debug_msg_static);
      //Serial.println(debug_msg_static);
    }
      
    
    
  }
}

// ------------------ SETUP ------------------

void setup() {
  virhas.setKpid(11.5, 1.6, 1.6);    //11.5 3.4  1.6
  virhas.setWheelRadius(wheel_radius);
  virhas.setEncoderPPR(encoder_ppr);
  virhas.setRobotRadius(robot_radius);
  virhas.stop();

  Serial.begin(9600);  // JSON from Jetson
  Serial.setTimeout(50); 

  alt=true;

  Serial.println("Arduino ready!");
}

// ------------------ LOOP ------------------

void loop() {
  Twist twist_msg;
  // --- LEGGE JSON DAL JETSON ---
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
  //if line is string
      if (!(line.startsWith("{") && line.endsWith("}"))){
        char cmd[20];   // deve essere abbastanza grande
        line.toCharArray(cmd, sizeof(cmd));

        if (line=="START"){
          alt=false;
          //Serial.println("[Arduino] received START");
        } 
        else if (line=="STOP"){
          alt=true;
          //Serial.println("[Arduino] received STOP");
        } 
        else Serial.println("[Arduino] Error, received invalid string\n");
        
      }
  //if line is a json  
    else {
      DynamicJsonDocument doc(200);
      DeserializationError error = deserializeJson(doc, line);
      if (!error) {
          twist_msg.linear_x  = doc["speed_x"] | 0.0;
          twist_msg.linear_y  = doc["speed_y"] | 0.0;
          twist_msg.angular_z = doc["angular_speed"] | 0.0;
      } else {
          Serial.print("[Arduino] JSON error: ");
          Serial.println(error.c_str());
      }
    }
  }
    moveRobot(&twist_msg);
  

}

  


// ------------------ SENSOR FUNCTIONS ------------------

void getSonarData() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    cm_prec4[i] = cm_prec3[i];
    cm_prec3[i] = cm_prec2[i];
    cm_prec2[i] = cm_prec1[i];
    cm_prec1[i] = cm_raw[i];
    cm_prec[i] = cm[i];
  }

  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    cm_raw[i] = sonar[i].ping_cm();
    numActualData = 0;

    if (cm_raw[i] == 0) {
      if (cm_prec1[i] + cm_prec2[i] + cm_prec3[i] + cm_prec4[i] == 0)
        cm[i] = MAX_DISTANCE;
      else {
        if (cm_prec1[i] != 0) numActualData++;
        if (cm_prec2[i] != 0) numActualData++;
        if (cm_prec3[i] != 0) numActualData++;
        if (cm_prec4[i] != 0) numActualData++;
        cm[i] = (cm_prec1[i] + cm_prec2[i] + cm_prec3[i] + cm_prec4[i]) / numActualData;
      }
    } else {
      if (cm_prec1[i] != 0) numActualData++;
      if (cm_prec2[i] != 0) numActualData++;
      if (cm_prec3[i] != 0) numActualData++;
      if (cm_prec4[i] != 0) numActualData++;
      cm[i] = (cm_raw[i] + cm_prec1[i] + cm_prec2[i] + cm_prec3[i] + cm_prec4[i]) / (numActualData + 1);
    }
  }
}

void publishSensorMsg() {
  getSonarData();
/*
  // Stampa i valori su seriale
  Serial.print("[Sonar] ");
  for (int i = 0; i < SONAR_NUM; i++) {
    Serial.print(cm[i]);
    if (i < SONAR_NUM - 1) Serial.print(", ");
  }
  Serial.println();
*/
/*
  Serial.print("[Odometry] X: ");
  Serial.print(virhas.getPosX());
  Serial.print(", Y: ");
  Serial.print(virhas.getPosY());
  Serial.print(", Theta: ");
  Serial.print(virhas.getPosTh());
  Serial.print(", Vx: ");
  Serial.print(virhas.getSpeedX());
  Serial.print(", Vy: ");
  Serial.print(virhas.getSpeedY());
  Serial.print(", W: ");
  Serial.println(virhas.getSpeedTh());
*/}
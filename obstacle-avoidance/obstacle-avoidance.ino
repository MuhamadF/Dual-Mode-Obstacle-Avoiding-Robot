/************************************************
    Manual control and obstacle avoidance robot
    (c) Muhamad F 2021

    Serial command
     *  A = Auto Mode
     *  S = Stop
     *  F = Forward
     *  B = Reverse
     *  L = Left
     *  R = Right
     *  M = Manual Mode
  
*************************************************/

#include <NewPing.h>

// BOUNDARY
#define MAX_DIST 100

// SENSOR PIN 1
int echoPin1 = 6;
int trigPin1 = 9; 

// SENSOR PIN 2
int echoPin2 = 5;
int trigPin2 = 11; 

// BUMPER PIN
int bumperPin = 12;

// MOTOR DRIVER PIN
int majuKanan = 2;
int mundurKanan = 4; 
int majuKiri = 7;
int mundurKiri  = 8;

// SPEEDCONTROL PIN
int kananSpeed = 3 ;  //PWM
int kiriSpeed = 10;  //PWM

// MOVEMENT VARIABLE
int speedSet = 110;
int bumperState;
int countLeft = 0;
int countRight = 0;
int jarakL = 0;
int jarakK = 0;

// BLUETOOTH VARIABLE
char btData;

// ULTRASONIC SENSOR LIBRARY DECLARATION
NewPing sonar1(trigPin1, echoPin1, MAX_DIST);
NewPing sonar2 (trigPin2, echoPin2, MAX_DIST);

/***************************************
    SETUP FUNCTION
***************************************/

void setup() {

 // SET PINMODE SENSOR
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT); 

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT); 

  pinMode(bumperPin, INPUT_PULLUP);

 // SET PINMODE MOTOR
  pinMode(majuKanan, OUTPUT);
  pinMode(majuKiri, OUTPUT);
  pinMode(mundurKanan, OUTPUT);
  pinMode(mundurKiri, OUTPUT);
  
  pinMode(kananSpeed, OUTPUT);
  pinMode(kiriSpeed, OUTPUT);

  InitialMark();
  
  // SERIAL FOR BLUETOOTH READINGS
  Serial.begin(9600);
}

/***************************************
    BASIC MOVEMENT
***************************************/

void InitialMark() {
  analogWrite(kiriSpeed, 60);
  analogWrite(kananSpeed, 60);

  Mundur(50);
  Berhenti(50);
  Mundur(50);
  Berhenti(50);
}

// FORWARD
void Maju() {
    digitalWrite(majuKanan, HIGH);
    digitalWrite(majuKiri, HIGH);
    digitalWrite(mundurKanan, LOW);
    digitalWrite(mundurKiri, LOW);
    
    analogWrite(kiriSpeed, speedSet);
    analogWrite(kananSpeed, speedSet);
}

//  BACKWARD
void Mundur(int d) {
    digitalWrite(majuKanan, LOW);
    digitalWrite(majuKiri, LOW);
    digitalWrite(mundurKanan, HIGH);
    digitalWrite(mundurKiri, HIGH);

    analogWrite(kiriSpeed, speedSet);
    analogWrite(kananSpeed, speedSet);

    delay(d);
}

// LEFT
void Kiri(int d) {
  digitalWrite(majuKanan, HIGH);
  digitalWrite(majuKiri, LOW);
  digitalWrite(mundurKanan, LOW);
  digitalWrite(mundurKiri, HIGH);
  
  analogWrite(kiriSpeed, speedSet);
  analogWrite(kananSpeed, speedSet);

  delay(d);
}

// RIGHT
void Kanan(int d) {
  digitalWrite(majuKanan, LOW);
  digitalWrite(majuKiri, HIGH);
  digitalWrite(mundurKanan, HIGH);
  digitalWrite(mundurKiri, LOW);

  analogWrite(kiriSpeed, speedSet);
  analogWrite(kananSpeed, speedSet);
  
  delay(d);
}

// STOP
void Berhenti(int d) {
  digitalWrite(majuKanan, LOW);
  digitalWrite(majuKiri, LOW);
  digitalWrite(mundurKanan, LOW);
  digitalWrite(mundurKiri, LOW);

  delay(d);
}

/***************************************
    SENSOR FUNCTION
***************************************/

// measuring distance using sensor
int readPing(NewPing namaSonar) { 
  int cm = namaSonar.ping_cm();
  if(cm==0)
  {
    cm = 1000;
  }
  return cm;
}

/***************************************
    MODE FUNCTION
***************************************/

// obstacle avoidance mode
void ObstacleAvoidMode() {
  Berhenti(300);
  
  while(1) {

    //WHEN 'M' RECEIVED, SWITCH TO MANUAL MODE
    if(Serial.available() > 0) {
    btData = Serial.read();
    }
    if(btData == 'M'){
      ManualMode();
    }

    bumperState = digitalRead(bumperPin);
    jarakK = readPing(sonar1);
    jarakL = readPing(sonar2);
    // IF THE ROBOT STUCK IN A CORNER
    // MOVE BACKWARD AFTER 2 TURN
    // READ SENSOR AGAIN, THEN TURN 
    // BASED FROM HIGHEST SENSOR VALUE
    if(countRight == 2 || countLeft == 2) {
       Mundur(700);
       jarakK = readPing(sonar1);
       jarakL = readPing(sonar2);
       Berhenti(150);
       if(jarakK < jarakL) {
          Kiri(400);
        } else {
          Kanan(400);
        }
       Berhenti(90);
       countRight = 0;
       countLeft = 0;
    }
    // STOP WHEN ONE SENSOR RETURNS <= 5CM
    // COMPARE 2 SENSOR READINGS, TURN THE ROBOT 
    //BASED FROM HIGHEST SENSOR VALUE
    if(jarakK <= 5 || jarakL <= 5 || bumperState == LOW) {
       Mundur(200);
       Berhenti(200);
      if(jarakK < jarakL) {
        Kiri(400);
        countLeft++;
      } else {
        Kanan(400);
        countRight++;
      }
      Berhenti(90);
    } else {
      Maju();
      countLeft = 0;
      countRight = 0;
    }
  }
}

// manual control mode
void ManualMode() {
  Berhenti(300);
  
  while(1) {

    // WHEN 'A' RECEIVED, SWITCH TO AUTONOMOUS MODE
    if(Serial.available() > 0) {
    btData = Serial.read();
    }
    if(btData == 'A'){
      ObstacleAvoidMode();
    }
     if(btData == 'S') {
          Berhenti(1);
     }
     if(btData == 'F') {
          Maju();
     }
     if(btData == 'B') { 
          Mundur(1);
     }
     if(btData == 'L') {
          Kiri(1);
     }
     if(btData == 'R') {
          Kanan(1);
     }
  }
}

/***************************************
    LOOP FUNCTION
***************************************/

void loop() {
  if(Serial.available() > 0) {
    btData = Serial.read();
  }

  switch(btData) {
    case 'M' :
      ManualMode();
      break;
    case 'A' :
      ObstacleAvoidMode();
      break;
    default :
      return;
      break;
  }
}

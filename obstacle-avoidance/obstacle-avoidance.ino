#include <NewPing.h>
#include <SoftwareSerial.h>

// BOUNDARY
#define MAX_DIST 100
#define MIN_DIST 15

// SENSOR PIN 1
int echoPin1 = 10;  // PWM
int trigPin1 = 11; 

// BUMPER PIN
int bumperPin = 2;

// BLUETOOTH PIN VIA SOFTWARE SERIAL
SoftwareSerial bt(12, 13); // RX / TX

// MOTOR DRIVER PIN
int majuKanan = 8;
int mundurKanan = 7; 
int majuKiri = 4;
int mundurKiri  = 3;

// SPEEDCONTROL PIN
int kananSpeed = 5 ;  //PWM
int kiriSpeed = 6;  //PWM
int brushSpeed = 9; // PWM

// MOVESET & SENSOR VARIABLE
int speedSet = 114;
int bumperState;
int jarakD = 0;
int jarakL = 0;
int jarakR = 0;
int dirStatus = 0;

// BLUETOOTH VARIABLE
char btData = 'A';

// ULTRASONIC SENSOR LIBRARY DECLARATION
NewPing sonar1(trigPin1, echoPin1, MAX_DIST);


void setup() {

 // SET PINMODE SENSOR
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT); 

 // SET PINMODE BUMPER
  pinMode(bumperPin, INPUT_PULLUP);

 // SET PINMODE MOTOR
  pinMode(majuKanan, OUTPUT);
  pinMode(majuKiri, OUTPUT);
  pinMode(mundurKanan, OUTPUT);
  pinMode(mundurKiri, OUTPUT);
  
  pinMode(kananSpeed, OUTPUT);
  pinMode(kiriSpeed, OUTPUT);
  pinMode(brushSpeed, OUTPUT);

  InitialMark();
  
  // SERIAL FOR BLUETOOTH READINGS
  Serial.begin(9600);
  bt.begin(9600);
}

void InitialMark() {
  analogWrite(kiriSpeed, 60);
  analogWrite(kananSpeed, 60);

  Mundur(50);
  Berhenti(50);
  Mundur(50);
  Berhenti(50);
}

// FORWARD
void Maju(int s) {
    digitalWrite(majuKanan, LOW);
    digitalWrite(majuKiri, LOW);
    digitalWrite(mundurKanan, HIGH);
    digitalWrite(mundurKiri, HIGH);

    analogWrite(kiriSpeed, s+10);
    analogWrite(kananSpeed, s);
}

//  BACKWARD
void Mundur(int d) {
    digitalWrite(majuKanan, HIGH);
    digitalWrite(majuKiri, HIGH);
    digitalWrite(mundurKanan, LOW);
    digitalWrite(mundurKiri, LOW);

    analogWrite(kiriSpeed, speedSet+10);
    analogWrite(kananSpeed, speedSet);
    delay(d);
}

// LEFT
void Kiri(int d) {
    digitalWrite(majuKanan, HIGH);
    digitalWrite(majuKiri, LOW);
    digitalWrite(mundurKanan, LOW);
    digitalWrite(mundurKiri, HIGH);

    analogWrite(kiriSpeed, speedSet+10);
    analogWrite(kananSpeed, speedSet);
    
    delay(d);
}

// RIGHT
void Kanan(int d) {
    digitalWrite(majuKanan, LOW);
    digitalWrite(majuKiri, HIGH);
    digitalWrite(mundurKanan, HIGH);
    digitalWrite(mundurKiri, LOW);

    analogWrite(kiriSpeed, speedSet+10);
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

// ULTRASONIC MEASURING FUNCTION
int readPing(NewPing namaSonar) { 
  int cm = namaSonar.ping_cm();
  if(cm <= 0)
  {
    cm = 30;
  }
  return cm;
}

// OBSTACLE AVOIDANCE MODE (DUMB)
void RandomMode() {
  Berhenti(300);
  
 while(1) {

    if(bt.available() > 0) {
    btData = bt.read();
    }
    if(btData == 'M'){
      ManualMode();
    }

    analogWrite(brushSpeed,40);
    jarakD = readPing(sonar1);
    bumperState = digitalRead(bumperPin);

    if(jarakD < 30) {
      Maju(95);
    }
    
    if(jarakD < MIN_DIST || bumperState == LOW) {
      Berhenti(100);
      Mundur(400);
      Berhenti(200);
      jarakR = readPing(sonar1);
      delay(100);
      jarakL = readPing(sonar1);
      delay(100);

      if(jarakL > jarakR) {
        Kanan(random(250, 450));
        Berhenti(200);
      }
      if(jarakL <= jarakR) {
        Kiri(random(250, 450));
        Berhenti(200);
      }
      if(jarakL < 10 && jarakR < 10) {
        Mundur(400);
        Berhenti(200);
      }
    } else {
      Maju(114);
    }
  }
}

// MANUAL MODE (SMARTPHONE)
void ManualMode() {
  Berhenti(300);
  
  while(1) {

    analogWrite(brushSpeed,40);

    if(bt.available() > 0) {
    btData = bt.read();
    }
     if(btData == 'A'){
        RandomMode();
     }
     if(btData == 'S') {
          Berhenti(1);
     }
     if(btData == 'F') {
          Maju(112);
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

void loop() {
  if(bt.available() > 0) {
    btData = bt.read();
  }

  switch(btData) {
    case 'M' :
      ManualMode();
      break;
    case 'A' :
      RandomMode();
      break;
    default :
      return;
      break;
  }
}

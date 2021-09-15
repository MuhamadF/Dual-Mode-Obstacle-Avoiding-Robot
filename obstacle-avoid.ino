#include <NewPing.h>


#define MAX_SPEED 105
#define MAX_DIST 100

// PIN SENSOR 1
int echoPin1 = 6;
int trigPin1 = 9; 

// PIN SENSOR 2
int echoPin2 = 5;
int trigPin2 = 11; 

// PIN DRIVER MOTOR
int majuKanan = 2;
int mundurKanan = 4; 
int majuKiri = 7;
int mundurKiri  = 8;

// PIN SPEED CONTROL
int kananSpeed = 3 ;  //PWM
int kiriSpeed = 10;  //PWM

// DEFINISI VARIABEL
int speedSet = 0;
int countLeft = 0;
int countRight = 0;
int jarakL = 0;
int jarakK = 0;

// VARIABEL BLUETOOTH
char btData;

boolean goesForward = false;

// DEKLARASI LIBRARY SENSOR
NewPing sonar1(trigPin1, echoPin1, MAX_DIST);
NewPing sonar2 (trigPin2, echoPin2, MAX_DIST);

void setup() {

 // SET PINMODE
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT); 

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT); 

  pinMode(majuKanan, OUTPUT);
  pinMode(majuKiri, OUTPUT);
  pinMode(mundurKanan, OUTPUT);
  pinMode(mundurKiri, OUTPUT);

  pinMode(kananSpeed, OUTPUT);
  pinMode(kiriSpeed, OUTPUT);

  // state asli = OBSTACLE AVOID
  btData = 'A';

  Serial.begin(9600);
}

void Maju() {
    digitalWrite(majuKanan, HIGH);
    digitalWrite(majuKiri, HIGH);
    digitalWrite(mundurKanan, LOW);
    digitalWrite(mundurKiri, LOW);

  // experimental
   // for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=1) {
      analogWrite(kiriSpeed, MAX_SPEED);
      analogWrite(kananSpeed, MAX_SPEED);
    //}
}

/*gerakan dasar*/

void Mundur(int d) {
    digitalWrite(majuKanan, LOW);
    digitalWrite(majuKiri, LOW);
    digitalWrite(mundurKanan, HIGH);
    digitalWrite(mundurKiri, HIGH);

  // experimental
 // for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=1) {
    analogWrite(kiriSpeed, MAX_SPEED);
    analogWrite(kananSpeed, MAX_SPEED);
   //}
  delay(d);
   
}

void Kiri(int d) {
  digitalWrite(majuKanan, HIGH);
  digitalWrite(majuKiri, LOW);
  digitalWrite(mundurKanan, LOW);
  digitalWrite(mundurKiri, HIGH);

  delay(d);
}

void Kanan(int d) {
  digitalWrite(majuKanan, LOW);
  digitalWrite(majuKiri, HIGH);
  digitalWrite(mundurKanan, HIGH);
  digitalWrite(mundurKiri, LOW);
  
  delay(d);
}

void Berhenti(int d) {
  digitalWrite(majuKanan, LOW);
  digitalWrite(majuKiri, LOW);
  digitalWrite(mundurKanan, LOW);
  digitalWrite(mundurKiri, LOW);

  delay(d);
}

int readPing(NewPing namaSonar) { 
  int cm = namaSonar.ping_cm();
  if(cm==0)
  {
    cm = 1000;
  }
  return cm;
}

void ObstacleAvoidMode() {
  Berhenti(300);
  
  while(1) {
    
    if(Serial.available() > 0) {
    btData = Serial.read();
    }
    if(btData == 'M'){
      ManualMode();
    }
    
    jarakK = readPing(sonar1);
    jarakL = readPing(sonar2);
    // Kalau stuck di sudut kanan & kiri
    // Mundur, Scan, Belok berdasarkan data sensor.
    if(countRight == 3 || countLeft == 3) {
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
    // Kalau didepan sensor kanan/kiri ada objek
    // belok berdasarkan data dari sensor
    if(jarakK <= 5 || jarakL <= 5) {
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

void ManualMode() {
  Berhenti(300);
  
  while(1) {
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

void loop() {
  if(Serial.available() > 0) {
    btData = Serial.read();
  }

  if(btData == 'M'){
    ManualMode();
  }
  if(btData == 'A') {
    ObstacleAvoidMode();
  }
}

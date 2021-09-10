#include <NewPing.h>

#define MAX_SPEED 95
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
}

void Maju() {
    digitalWrite(majuKanan, HIGH);
    digitalWrite(majuKiri, HIGH);
    digitalWrite(mundurKanan, LOW);
    digitalWrite(mundurKiri, LOW);

  // experimental
    for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=1) {
      analogWrite(kiriSpeed, speedSet);
      analogWrite(kananSpeed, speedSet);
    }
}

void Mundur(int d) {
    digitalWrite(majuKanan, LOW);
    digitalWrite(majuKiri, LOW);
    digitalWrite(mundurKanan, HIGH);
    digitalWrite(mundurKiri, HIGH);

  // experimental
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=1) {
    analogWrite(kiriSpeed, speedSet);
    analogWrite(kananSpeed, speedSet);
   }
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
  Serial.println(cm);
  return cm;
}

void loop() {
  jarakK = readPing(sonar1);
  jarakL = readPing(sonar2);
    // Kalau stuck di sudut kanan & kiri
    // Mundur, Scan, Belok berdasarkan data sensor.
    if(countRight == 2 || countLeft == 2) {
       Berhenti(200);
       Mundur(400);
       jarakK = readPing(sonar1);
       jarakL = readPing(sonar2);
       Berhenti(200);
       if(jarakK < jarakL) {
          Kiri(320);
        } else {
          Kanan(320);
        }
        Berhenti(200);
       countRight = 0;
       countLeft = 0;
    }
    // Kalau didepan sensor kanan/kiri ada objek
    // belok berdasarkan data dari sensor
    if(jarakK <= 10 || jarakL <= 10) {
       Berhenti(200);
      if(jarakK < jarakL) {
        Kiri(320);
        countLeft++;
      } else {
        Kanan(320);
        countRight++;
      }
      Berhenti(300);
    } else {
      Maju();
      countLeft = 0;
      countRight = 0;
    }
}

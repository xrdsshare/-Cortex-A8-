#include"qudong.h"
#include <Servo.h>   
Servo myservo;

void forward(int interval=0){
  digitalWrite(_L1, HIGH);
  digitalWrite(_L2, LOW);
  digitalWrite(_R1, HIGH);
  digitalWrite(_R2, LOW);
  delay(interval);
}
void backward(int interval=500) {
  digitalWrite(_L1, LOW);
  digitalWrite(_L2, HIGH);
  digitalWrite(_R1, LOW);
  digitalWrite(_R2, HIGH);
  delay(interval);
}
 void leftForward(int interval=250) {
  digitalWrite(_L1, LOW);
  digitalWrite(_L2, LOW);
  digitalWrite(_R1, HIGH);
  digitalWrite(_R2, LOW);
  delay(interval);
}
void rightForward(int interval=250) {
  digitalWrite(_L1, HIGH);
  digitalWrite(_L2, LOW);
  digitalWrite(_R1, LOW);
  digitalWrite(_R2, LOW);
  delay(interval);
}
void leftRotate(int interval=500){
  digitalWrite(_L1,LOW);
  digitalWrite(_L2,HIGH);
  digitalWrite(_R1,HIGH);
  digitalWrite(_R2,LOW);
  delay(interval);
}
void rightRotate(int interval=500){
  digitalWrite(_L1,HIGH);
  digitalWrite(_L2,LOW);
  digitalWrite(_R1,LOW);
  digitalWrite(_R2,HIGH);
  delay(interval);
}
void djstop(int interval=10) {
  digitalWrite(_L1,LOW);
  digitalWrite(_L2,LOW);
  digitalWrite(_R1,LOW);
  digitalWrite(_R2,LOW);
  delay(interval);
}
void sg90(int sangle, int stime){
    if(sangle>10 && sangle<170){
      myservo.write(sangle);              // tell servo to go to position in variable 'pos'
      delay(stime);
    }
}

void dianji(int mod, int dtime){
  switch(mod){
    case sgw:
      sg90(dtime, 15);
      break;
    case mfw:
      forward(dtime);
      break;
    case mbw:
      backward(dtime);
      break;
    case lfw:
      leftForward(dtime);
      break;
    case rfw:
      rightForward(dtime);
      break;
    case lrt:
      leftRotate(dtime);
      break;
    case rrt:
      rightRotate(dtime);
      break;
    case djs:
      djstop(dtime);
      break;
    default :
      break;
  }
}

void setup() {
  Serial.begin(115200);  
  inputString.reserve(200);
  myservo.attach(_sg);
}

void loop() {
 // forward(1000);
 if(stringComplete == true){
    Serial.println(valmod);
    inputString = "";    
    stringComplete = false;

    dianji(valmod, valtime);
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if(inChar != '|' && inChar != '\n'){
      inputString += inChar;
    }else if (inChar == '|') {
      valmod = inputString.toInt();
      inputString = "";
    }else if (inChar == '\n') {
      valtime = inputString.toInt();
      inputString = "";
      stringComplete = true;
    }
  }
}

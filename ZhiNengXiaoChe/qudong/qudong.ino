#define _L1 6
#define _L2 7                                    //右侧电机ENABLE
#define _R1 8
#define _R2 9      

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
  digitalWrite(_R1, HIGH);
  digitalWrite(_R2, LOW);
  delay(interval);
}
void rightForward(int interval=250) {
  digitalWrite(_L1, HIGH);
  digitalWrite(_L2, LOW);
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
void stop(int interval=10) {
  analogWrite(_ENL, 0);
  analogWrite(_ENR, 0);
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  forward(1000);
}



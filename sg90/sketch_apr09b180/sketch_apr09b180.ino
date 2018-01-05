#include <Servo.h>  
  
#define PIN_SERVO 10  
Servo myservo;  
  
void setup()  
{  
  myservo.attach(PIN_SERVO);  
}  
  
void loop()  
{  
  myservo.write(90);   
}  

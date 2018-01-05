int servopin = 7;    //定义舵机接口数字接口7 也就是舵机的橙色信号线。
 
void servopulse(int angle)//定义一个脉冲函数
{
  int pulsewidth=(angle*11)+500;  //将角度转化为500-2480的脉宽值
  digitalWrite(servopin,HIGH);    //将舵机接口电平至高
  delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
  digitalWrite(servopin,LOW);     //将舵机接口电平至低
  delayMicroseconds(20000-pulsewidth);
}
 
void setup()
{
  pinMode(servopin,OUTPUT);//设定舵机接口为输出接口
}
 
void loop()
{
    //把值的范围映射到0到165左右
 for( int angle = 0;angle<165;angle+=10){
    for(int i=0;i<50;i++)  //发送50个脉冲
  {
       servopulse(angle);   //引用脉冲函数
  }
  delay(1000);
 }
}

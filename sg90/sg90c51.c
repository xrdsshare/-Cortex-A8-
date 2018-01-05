#include <reg52.h>  
#define Stop 0    //宏定义，停止 
#define Left 1   //宏定义，左转 
#define Right 2   //宏定义，右转    
sbit ControlPort = P2^0;  //舵机信号端口 
sbit KeyLeft = P1^0;   //左转按键端口 
sbit KeyRight = P1^1;   //右转按键端口 
sbit KeyStop = P1^2;   //归位按键端口   
unsigned char TimeOutCounter = 0,LeftOrRight = 0;  //TimeOutCounter：定时器溢出计数   LeftOrRight：舵机左右旋转标志   
void InitialTimer ( void )   
{      
TMOD=0x10;   //定时/计数器1工作于方式1     
TH1 = ( 65535 - 500 ) / 256;   //0.25ms  
TL1 = ( 65535 - 500 ) % 256;     
EA=1;        //开总中断      
ET1=1;       //允许定时/计数器1 中断     
TR1=1;       //启动定时/计数器1 中断
 }  
void ControlLeftOrRight ( void )   //控制舵机函数 
{  if( KeyStop == 0 )  
{   //while ( !KeyStop );    //使标志等于Stop（0），在中断函数中将用到   
LeftOrRight = Stop;  
}   
if( KeyLeft == 0 )  
{   //while ( !KeyLeft );    //使标志等于Left（1），在中断函数中将用到 
  LeftOrRight = Left; 
 }   
if( KeyRight == 0 ) 
 {   //while ( !KeyRight );   //使标志等于Right（2），在中断函数中将用到  
 LeftOrRight = Right; 
 }  
 tob_id_4526
}  
 void main ( void )  //主函数 
{     
 InitialTimer(); 
 for(;;) 
 {  
 ControlLeftOrRight(); 
 }
 }
   void Timer1 ( void ) interrupt 3  //定时器中断函数 
{
      TH1 = ( 65535 - 500 ) / 256;
  TL1 = ( 65535 - 500 ) % 256;
  TimeOutCounter ++;
   switch ( LeftOrRight ) 
 { 
  case 0 :   //为0时，舵机归位，脉宽1.5ms   
{
    if( TimeOutCounter <= 6 ) 
   {
     ControlPort = 1;   
 }
    else     
{ 
    ControlPort = 0;   
 } 
   break;  
 }
   case 1 :     //为1时，舵机左转，脉宽1ms（理论值），实际可以调试得出 
  { 
   if( TimeOutCounter <= 2 ) 
   { 
    ControlPort = 1;
    } 
   else 
    { 
    ControlPort = 0; 
   }
    break; 
  } 
  case 2 :   //为2时，舵机右转，脉宽2ms（理论值），实际可以调试得出 
  {
    if( TimeOutCounter <= 10 ) 
   { 
    ControlPort = 1;   
 }
    else   
  {  
   ControlPort = 0;   
 }
    break; 
  } 
  default : break; 
 }
     if( TimeOutCounter == 80 )  //周期20ms（理论值），比较可靠，最好不要修改 
 { 
  TimeOutCounter = 0;
  }
  }

#include <reg52.h>  
#define Stop 0    //�궨�壬ֹͣ 
#define Left 1   //�궨�壬��ת 
#define Right 2   //�궨�壬��ת    
sbit ControlPort = P2^0;  //����źŶ˿� 
sbit KeyLeft = P1^0;   //��ת�����˿� 
sbit KeyRight = P1^1;   //��ת�����˿� 
sbit KeyStop = P1^2;   //��λ�����˿�   
unsigned char TimeOutCounter = 0,LeftOrRight = 0;  //TimeOutCounter����ʱ���������   LeftOrRight�����������ת��־   
void InitialTimer ( void )   
{      
TMOD=0x10;   //��ʱ/������1�����ڷ�ʽ1     
TH1 = ( 65535 - 500 ) / 256;   //0.25ms  
TL1 = ( 65535 - 500 ) % 256;     
EA=1;        //�����ж�      
ET1=1;       //����ʱ/������1 �ж�     
TR1=1;       //������ʱ/������1 �ж�
 }  
void ControlLeftOrRight ( void )   //���ƶ������ 
{  if( KeyStop == 0 )  
{   //while ( !KeyStop );    //ʹ��־����Stop��0�������жϺ����н��õ�   
LeftOrRight = Stop;  
}   
if( KeyLeft == 0 )  
{   //while ( !KeyLeft );    //ʹ��־����Left��1�������жϺ����н��õ� 
  LeftOrRight = Left; 
 }   
if( KeyRight == 0 ) 
 {   //while ( !KeyRight );   //ʹ��־����Right��2�������жϺ����н��õ�  
 LeftOrRight = Right; 
 }  
 tob_id_4526
}  
 void main ( void )  //������ 
{     
 InitialTimer(); 
 for(;;) 
 {  
 ControlLeftOrRight(); 
 }
 }
   void Timer1 ( void ) interrupt 3  //��ʱ���жϺ��� 
{
      TH1 = ( 65535 - 500 ) / 256;
  TL1 = ( 65535 - 500 ) % 256;
  TimeOutCounter ++;
   switch ( LeftOrRight ) 
 { 
  case 0 :   //Ϊ0ʱ�������λ������1.5ms   
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
   case 1 :     //Ϊ1ʱ�������ת������1ms������ֵ����ʵ�ʿ��Ե��Եó� 
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
  case 2 :   //Ϊ2ʱ�������ת������2ms������ֵ����ʵ�ʿ��Ե��Եó� 
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
     if( TimeOutCounter == 80 )  //����20ms������ֵ�����ȽϿɿ�����ò�Ҫ�޸� 
 { 
  TimeOutCounter = 0;
  }
  }

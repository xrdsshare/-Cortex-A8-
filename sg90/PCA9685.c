/**************************************************************************
                        PCA9685ģ���Ӧ��
                ƽ̨��89C52������11.0592
***************************************************************************/
#include<reg52.h>           
#include <intrins.h>  
#include <stdio.h>
#include <math.h>
typedef  unsigned char  uchar;        
typedef  unsigned int   uint;        


sbit scl=P3^6;                        //ʱ��������
sbit sda=P3^7;                   //��������/�����


#define PCA9685_adrr 0x80//  1+A5+A4+A3+A2+A1+A0+w/r 
                                            //Ƭѡ��ַ�������ӵ���1�ɸı��ַ��
                                            //        ��IIC�������ж�ƬPCA9685����ͬ��ַʱ���躸��
#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4


#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE


#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9


#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD


#define SERVOMIN  115 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  590 // this is the 'maximum' pulse length count (out of 4096)
#define SERVO000  130 //0�ȶ�Ӧ4096���������ֵ
#define SERVO180  520 //180�ȶ�Ӧ4096���������ֵ���ĸ�ֵ�ɸ��ݲ�ͬ����޸�


/**********************����������*********************************/
/*---------------------------------------------------------------
                  ������ʱ����
----------------------------------------------------------------*/
void delayms(uint z)
{
  uint x,y;
  for(x=z;x>0;x--)
      for(y=148;y>0;y--);
}
/*---------------------------------------------------------------
                                                                        IIC���������ͨ�ú���
----------------------------------------------------------------*/
/*---------------------------------------------------------------
                 ΢�����ʱ���� ����4.7us
----------------------------------------------------------------*/
void delayus()
{
          _nop_();          //��intrins.h�ļ���
                _nop_();
                _nop_();
                _nop_();
                _nop_();

}
/*---------------------------------------------------------------
                 IIC���߳�ʼ������ 
----------------------------------------------------------------*/
void init()
{
    sda=1;                //sda sclʹ��ǰ���Ǳ�����
    delayus();
    scl=1;
    delayus();
}
/*---------------------------------------------------------------
                 IIC���������źź���
----------------------------------------------------------------*/
void start()
{
    sda=1;
    delayus();
    scl=1;                        //scl����ʱ sdaͻȻ�����͵�ƽ ��������IIC����
    delayus();
    sda=0;
    delayus();
                scl=0;
                delayus();
}
/*---------------------------------------------------------------
                 IIC����ֹͣ�źź���
----------------------------------------------------------------*/
void stop()
{
    sda=0;
    delayus();
    scl=1;                         //scl����ʱ sdaͻȻ�����ߵ�ƽ ��ֹͣ��IIC����
    delayus();
    sda=1;                   
    delayus();
}
/*---------------------------------------------------------------
                 IIC����Ӧ���źź���
----------------------------------------------------------------*/
void ACK()
{
    uchar i;
    scl=1;
    delayus();
    while((sda=1)&&(i<255))         
                i++;                                        
    scl=0;                                  
    delayus();
}
/*---------------------------------------------------------------
                 дһ���ֽ�,�޷���ֵ��������һ���ֽ�ֵ
----------------------------------------------------------------*/
void write_byte(uchar byte)
{
    uchar i,temp;
    temp=byte;
    for(i=0;i<8;i++)
    {
        temp=temp<<1;  
        scl=0;                  
                                delayus();
                                sda=CY;                 
                                delayus();
                                scl=1;           
                                delayus();
    }
    scl=0;                  
    delayus();
    sda=1;                 
    delayus();
}
/*---------------------------------------------------------------
                 ��һ���ֽں������з���ֵ
----------------------------------------------------------------*/
uchar read_byte()
{
                uchar i,j,k;
                scl=0;
                delayus();
                sda=1;
                delayus();
                for(i=0;i<8;i++)        
                {
                                delayus();
                                scl=1;
                delayus();
                if(sda==1)
                {
                                j=1;
                }
                else j=0;
                k=(k<< 1)|j;  
                scl=0;            
                }
                delayus();
                return k;
}
/*---------------------------------------------------------------
                �й�PCA9685ģ��ĺ���
----------------------------------------------------------------*/
/*---------------------------------------------------------------
                ��PCA9685��д��ַ������
----------------------------------------------------------------*/
void PCA9685_write(uchar address,uchar date)
{
                start();
                write_byte(PCA9685_adrr);        //PCA9685��Ƭѡ��ַ
                ACK();                          
                write_byte(address);  //д��ַ�����ֽ�
                ACK();
                write_byte(date);          //д����
                ACK();
                stop();
}
/*---------------------------------------------------------------
            ��PCA9685��ĵ�ֵַ�ж�����(�з���ֵ)
----------------------------------------------------------------*/
uchar PCA9685_read(uchar address)
{
                uchar date;
                start();
                write_byte(PCA9685_adrr); //PCA9685��Ƭѡ��ַ
                ACK();
                write_byte(address);
                ACK();
                start();
                write_byte(PCA9685_adrr|0x01);        //��ַ�ĵڰ�λ�������������򣬾���д���
                ACK();
                date=read_byte();
                stop();
                return date;
}
/*---------------------------------------------------------------
                        PCA9685��λ
----------------------------------------------------------------*/
void reset(void) 
{
                PCA9685_write(PCA9685_MODE1,0x0);
}


void begin(void) 
{
                reset();
}
/*---------------------------------------------------------------
                                        PCA9685�޸�Ƶ�ʺ���
----------------------------------------------------------------*/
void setPWMFreq(float freq) 
{
                uint prescale,oldmode,newmode;
                float prescaleval;
                freq *= 0.92;  // Correct for overshoot in the frequency setting 
                prescaleval = 25000000;
                prescaleval /= 4096;
                prescaleval /= freq;
                prescaleval -= 1;
                prescale = floor(prescaleval + 0.5);
                
                oldmode = PCA9685_read(PCA9685_MODE1);
                newmode = (oldmode&0x7F) | 0x10; // sleep
                PCA9685_write(PCA9685_MODE1, newmode); // go to sleep
                PCA9685_write(PCA9685_PRESCALE, prescale); // set the prescaler
                PCA9685_write(PCA9685_MODE1, oldmode);
                delayms(2);
                PCA9685_write(PCA9685_MODE1, oldmode | 0xa1); 
}
/*---------------------------------------------------------------
                                PCA9685�޸ĽǶȺ���
num:���PWM�������0~15��on:PWM��������ֵ0~4096,off:PWM�½�����ֵ0~4096
һ��PWM���ڷֳ�4096�ݣ���0��ʼ+1�������Ƶ�onʱ����Ϊ�ߵ�ƽ������������offʱ
����Ϊ�͵�ƽ��ֱ������4096���¿�ʼ�����Ե�on������0ʱ������ʱ,��on����0ʱ��
off/4096��ֵ����PWM��ռ�ձȡ�
----------------------------------------------------------------*/
void setPWM(uint num, uint on, uint off) 
{
                PCA9685_write(LED0_ON_L+4*num,on);
                PCA9685_write(LED0_ON_H+4*num,on>>8);
                PCA9685_write(LED0_OFF_L+4*num,off);
                PCA9685_write(LED0_OFF_H+4*num,off>>8);
        }


/*---------------------------------------------------------------
                      ������
----------------------------------------------------------------*/
void main()
{
                begin();
                setPWMFreq(50);  
                //����Ҫ����ת��60�ȣ���ô�㣬
                //60�ȶ�Ӧ������=0.5ms+(60/180)*(2.5ms-0.5ms)=1.1666ms
                //����ռ�ձ�=1.1666ms/20ms=off/4096,off=239,50hz��Ӧ����20ms
                //setPWM(num,0,239);;;;��ȻҲ��������SERVO000��SERVO180����
                while(1) 
                {
                                setPWM(0, 0, SERVOMIN);//��0·���ת����С�Ƕ�
                                setPWM(1, 0, SERVO000);//��1·���ת��0�Ƕ�
                                delayms(1500);
                                setPWM(0, 0, SERVOMAX);
                                setPWM(1, 0, SERVO180);
                                delayms(1500);
                }                
}


/*---------------------------------------------------------------
                      END
----------------------------------------------------------------*/
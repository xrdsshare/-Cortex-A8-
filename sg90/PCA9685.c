/**************************************************************************
                        PCA9685模块简单应用
                平台：89C52，晶振：11.0592
***************************************************************************/
#include<reg52.h>           
#include <intrins.h>  
#include <stdio.h>
#include <math.h>
typedef  unsigned char  uchar;        
typedef  unsigned int   uint;        


sbit scl=P3^6;                        //时钟输入线
sbit sda=P3^7;                   //数据输入/输出端


#define PCA9685_adrr 0x80//  1+A5+A4+A3+A2+A1+A0+w/r 
                                            //片选地址，将焊接点置1可改变地址，
                                            //        当IIC总线上有多片PCA9685或相同地址时才需焊接
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
#define SERVO000  130 //0度对应4096的脉宽计数值
#define SERVO180  520 //180度对应4096的脉宽计算值，四个值可根据不同舵机修改


/**********************函数的声明*********************************/
/*---------------------------------------------------------------
                  毫秒延时函数
----------------------------------------------------------------*/
void delayms(uint z)
{
  uint x,y;
  for(x=z;x>0;x--)
      for(y=148;y>0;y--);
}
/*---------------------------------------------------------------
                                                                        IIC总线所需的通用函数
----------------------------------------------------------------*/
/*---------------------------------------------------------------
                 微妙级别延时函数 大于4.7us
----------------------------------------------------------------*/
void delayus()
{
          _nop_();          //在intrins.h文件里
                _nop_();
                _nop_();
                _nop_();
                _nop_();

}
/*---------------------------------------------------------------
                 IIC总线初始化函数 
----------------------------------------------------------------*/
void init()
{
    sda=1;                //sda scl使用前总是被拉高
    delayus();
    scl=1;
    delayus();
}
/*---------------------------------------------------------------
                 IIC总线启动信号函数
----------------------------------------------------------------*/
void start()
{
    sda=1;
    delayus();
    scl=1;                        //scl拉高时 sda突然来个低电平 就启动了IIC总线
    delayus();
    sda=0;
    delayus();
                scl=0;
                delayus();
}
/*---------------------------------------------------------------
                 IIC总线停止信号函数
----------------------------------------------------------------*/
void stop()
{
    sda=0;
    delayus();
    scl=1;                         //scl拉高时 sda突然来个高电平 就停止了IIC总线
    delayus();
    sda=1;                   
    delayus();
}
/*---------------------------------------------------------------
                 IIC总线应答信号函数
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
                 写一个字节,无返回值，需输入一个字节值
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
                 读一个字节函数，有返回值
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
                有关PCA9685模块的函数
----------------------------------------------------------------*/
/*---------------------------------------------------------------
                向PCA9685里写地址，数据
----------------------------------------------------------------*/
void PCA9685_write(uchar address,uchar date)
{
                start();
                write_byte(PCA9685_adrr);        //PCA9685的片选地址
                ACK();                          
                write_byte(address);  //写地址控制字节
                ACK();
                write_byte(date);          //写数据
                ACK();
                stop();
}
/*---------------------------------------------------------------
            从PCA9685里的地址值中读数据(有返回值)
----------------------------------------------------------------*/
uchar PCA9685_read(uchar address)
{
                uchar date;
                start();
                write_byte(PCA9685_adrr); //PCA9685的片选地址
                ACK();
                write_byte(address);
                ACK();
                start();
                write_byte(PCA9685_adrr|0x01);        //地址的第八位控制数据流方向，就是写或读
                ACK();
                date=read_byte();
                stop();
                return date;
}
/*---------------------------------------------------------------
                        PCA9685复位
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
                                        PCA9685修改频率函数
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
                                PCA9685修改角度函数
num:舵机PWM输出引脚0~15，on:PWM上升计数值0~4096,off:PWM下降计数值0~4096
一个PWM周期分成4096份，由0开始+1计数，计到on时跳变为高电平，继续计数到off时
跳变为低电平，直到计满4096重新开始。所以当on不等于0时可作延时,当on等于0时，
off/4096的值就是PWM的占空比。
----------------------------------------------------------------*/
void setPWM(uint num, uint on, uint off) 
{
                PCA9685_write(LED0_ON_L+4*num,on);
                PCA9685_write(LED0_ON_H+4*num,on>>8);
                PCA9685_write(LED0_OFF_L+4*num,off);
                PCA9685_write(LED0_OFF_H+4*num,off>>8);
        }


/*---------------------------------------------------------------
                      主函数
----------------------------------------------------------------*/
void main()
{
                begin();
                setPWMFreq(50);  
                //例如要求舵机转到60度，这么算，
                //60度对应的脉宽=0.5ms+(60/180)*(2.5ms-0.5ms)=1.1666ms
                //利用占空比=1.1666ms/20ms=off/4096,off=239,50hz对应周期20ms
                //setPWM(num,0,239);;;;当然也可以利用SERVO000和SERVO180计算
                while(1) 
                {
                                setPWM(0, 0, SERVOMIN);//第0路舵机转到最小角度
                                setPWM(1, 0, SERVO000);//第1路舵机转到0角度
                                delayms(1500);
                                setPWM(0, 0, SERVOMAX);
                                setPWM(1, 0, SERVO180);
                                delayms(1500);
                }                
}


/*---------------------------------------------------------------
                      END
----------------------------------------------------------------*/
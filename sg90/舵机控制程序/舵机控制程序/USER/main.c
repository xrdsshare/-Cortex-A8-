#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "pwm.h"

//Mini STM32开发板范例代码8
//PWM输出 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com	 
 int main(void)
 {
	u16 led0pwmval=0;
	u8 dir=1;
	 u8 i;
	SystemInit();
	delay_init(72);	     //延时初始化
	NVIC_Configuration();
	uart_init(9600);
 	LED_Init();
	KEY_Init();          //按键初始化	
	PWM_Init(199,7199);	 //20ms的脉冲  一个脉2us 
	    while(1)
   {          
         TIM_SetCompare2(TIM3,5); 
         delay_ms(300);
         TIM_SetCompare2(TIM3,10); 
         delay_ms(300);
         TIM_SetCompare2(TIM3,15); 
         delay_ms(300);
         TIM_SetCompare2(TIM3,20); 
         delay_ms(300);//
		 
         TIM_SetCompare2(TIM3,15); 
         delay_ms(300);
		 TIM_SetCompare2(TIM3,10); 
         delay_ms(300);
	
   }  
	 
	/*	while(1)
	{
 		delay_ms(10);	
			for(i=1;i<1000;i++)
			{
		if(dir)led0pwmval++;
		else led0pwmval--;
			}
		if(led0pwmval==10000)dir=0;	
 	//	if(led0pwmval>2500)led0pwmval=500;
		if(led0pwmval==0)dir=1;	
	
												 
		TIM_SetCompare2(TIM3,led0pwmval);	   
	}	 
	*/

 }


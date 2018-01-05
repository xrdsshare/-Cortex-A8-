#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "pwm.h"

//Mini STM32�����巶������8
//PWM��� ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com	 
 int main(void)
 {
	u16 led0pwmval=0;
	u8 dir=1;
	 u8 i;
	SystemInit();
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();
	uart_init(9600);
 	LED_Init();
	KEY_Init();          //������ʼ��	
	PWM_Init(199,7199);	 //20ms������  һ����2us 
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


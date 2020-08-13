#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "stm32f10x_adc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include "motor.h"


int PW = 0;//统计（LED）脉冲的基本单位，10us个数在中断函数会被初始化为0，static也没用.
int MotorPW = 0;
int Lim_PW = 0;//统计脉冲宽度，控制几个10us宽度
int Lim_MotorPW = 0;//统计脉冲宽度，控制几个10us宽度

void UsartSignalToLED_model(char UsrtSignal,short OpenCloseFlag);
int UsrtSignalToBrigtness(char UsrtSignal);

/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
*******************************************************************************/
void  Delay (uint32_t nCount){
  
	for(; nCount != 0; nCount--);
}


int main(void){

//	char uart_signal = '0';
//	short i = 0;
	LED_Init();		  	//初始化与LED连接的硬件接口
	Motor_Init();
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	TIM3_Int_Init(100-1,72-1);//72MHz的计数频率，Tout=(arr+1)(psc+1)/TIM3的时钟72MHz,100us=0.1ms溢出一次.
	RedLedOpen(); 
	
	 
	while(1){
		
		Lim_PW = UsrtSignalToBrigtness('2');//0 1 2 3 4 5 6 7 8 9 s更改灯的亮度

		Lim_MotorPW = 13; //5 10 15 20 25(25会抖动，用24代替)
								//0.5 1 1.5 2 2.5ms
								//-90 -45 0 45 90°
	   //MotorUp();
		//PBout(7)=1;
			
		}
}

	
 
 
 /*******************************************************************************
* Function Name  : TIM3中断(服务函数)
* Description    : 在某灯输出PWM信号
* Input          : None
* Output         : None
* Return         : None
* Attention		 : PW个10us,其中<Lim_PW输出+，反之-，波的周期10us*PW
*******************************************************************************/
 void TIM3_IRQHandler(void){   

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){ //检查指定的TIM中断发生与否:TIM 中断源 
		
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 

//-------------中断服务函数-------------------------------------		
		//-----判断啥时候到LED脉冲边缘--------
		if(PW < Lim_PW){
			
		UsartSignalToLED_model('z',1);
				
		}
		
		if(PW >= Lim_PW){ 
				
		UsartSignalToLED_model('z',0);
				
		}
		//-----判断啥时候到舵机脉冲边缘（20ms / 0 4 8 12 16 20 ms）--------
		if(PW < Lim_MotorPW){
			
		//舵机io高电平
		MotorUp();
				
		}
		
		if(PW >= Lim_MotorPW){ 
				
		//舵机io低电平
		MotorDown();
				
		}
	  
		PW++;//100us=0.1ms增加一次
		if(PW == 100)PW=0;//10ms
			
		MotorPW++;//100us=0.1ms增加一次
		if(MotorPW == 200)MotorPW = 0;//20ms
	
	}
}

 /*******************************************************************************
* Function Name  : UsartSignalToLED_model
* Description    : 根据得到的串口信号决定那几颗灯亮，现在先用一个固定值代替。
* Input          : UsrtSignal用户串口信号 OpenCloseFlag返回是开开始关的动作（PWM所需）
* Output         : 几颗灯亮
* Return         : None
* Attention		 : OpenCloseFlag = 1  对应开的信号
*******************************************************************************/
void UsartSignalToLED_model(char UsrtSignal,short OpenCloseFlag){
	
	switch(UsrtSignal){
	
		case 'z' :{if(OpenCloseFlag)ProgramLedOpen();//测试用的，程序指示灯
						else ProgramLedClose();}break;
		case 'a' :{if(OpenCloseFlag);//要修改成所有灯关闭
						else ;}break;
		case 'c' :{if(OpenCloseFlag)BlueLedOpen();
						else            BlueLedClose();}break;
		case 'd' :{if(OpenCloseFlag)GreenLedOpen();
						else            GreenLedClose();}break;
//		case 'e' :{if(OpenCloseFlag)BlueLedOpen();
//						else            BlueLedOpen();}break;
//		case 'f' :{if(OpenCloseFlag)BlueLedOpen();
//						else            BlueLedOpen();}break;
		
						
						
						
	}
		
}

 /*******************************************************************************
* Function Name  : UsartSignalToBrigtness
* Description    : 用户窗口信号转成亮度指标Lim_PW
* Input          : UsrtSignal
* Output         : 亮度指标(pwm占空数)
* Return         : Lim_PW
* Attention		 : 
*******************************************************************************/
int UsrtSignalToBrigtness(char UsrtSignal){
	
	switch(UsrtSignal){
	
		case '1' : return 10;break;
		case '2' : return 20;break;
		case '3' : return 30;break;
		case '4' : return 40;break;
		case '5' : return 50;break;
		case '6' : return 60;break;
		case '7' : return 70;break;
		case '8' : return 80;break;
		case '9' : return 90;break;
		case 's' : return 100;break;
		default : return 0;
	}
}



/*-------------------代码垃圾桶--------------------------------------
RedLed1Close();//副红灯关
		ProgramLedOpen();//程序指示LED开
		BlueLedOpen();//蓝灯开
		PBout(6)=1;
		PBout(10)=1;
		_750LedClose();
		delay_ms(1500);
		RedLed1Open();//副红灯开
		ProgramLedClose();//程序指示LED关
		_750LedOpen();
		BlueLedClose();//蓝灯关
		PBout(6)=0;
		PBout(10)=0;
		delay_ms(1500);
 
 
---------------------代码垃圾桶--------------------------------------*/






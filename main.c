#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "stm32f10x_adc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include "motor.h"


int PW = 0;//ͳ�ƣ�LED������Ļ�����λ��10us�������жϺ����ᱻ��ʼ��Ϊ0��staticҲû��.
int MotorPW = 0;
int Lim_PW = 0;//ͳ�������ȣ����Ƽ���10us���
int Lim_MotorPW = 0;//ͳ�������ȣ����Ƽ���10us���

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
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	Motor_Init();
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	TIM3_Int_Init(100-1,72-1);//72MHz�ļ���Ƶ�ʣ�Tout=(arr+1)(psc+1)/TIM3��ʱ��72MHz,100us=0.1ms���һ��.
	RedLedOpen(); 
	
	 
	while(1){
		
		Lim_PW = UsrtSignalToBrigtness('2');//0 1 2 3 4 5 6 7 8 9 s���ĵƵ�����

		Lim_MotorPW = 13; //5 10 15 20 25(25�ᶶ������24����)
								//0.5 1 1.5 2 2.5ms
								//-90 -45 0 45 90��
	   //MotorUp();
		//PBout(7)=1;
			
		}
}

	
 
 
 /*******************************************************************************
* Function Name  : TIM3�ж�(������)
* Description    : ��ĳ�����PWM�ź�
* Input          : None
* Output         : None
* Return         : None
* Attention		 : PW��10us,����<Lim_PW���+����֮-����������10us*PW
*******************************************************************************/
 void TIM3_IRQHandler(void){   

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){ //���ָ����TIM�жϷ������:TIM �ж�Դ 
		
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 

//-------------�жϷ�����-------------------------------------		
		//-----�ж�ɶʱ��LED�����Ե--------
		if(PW < Lim_PW){
			
		UsartSignalToLED_model('z',1);
				
		}
		
		if(PW >= Lim_PW){ 
				
		UsartSignalToLED_model('z',0);
				
		}
		//-----�ж�ɶʱ�򵽶�������Ե��20ms / 0 4 8 12 16 20 ms��--------
		if(PW < Lim_MotorPW){
			
		//���io�ߵ�ƽ
		MotorUp();
				
		}
		
		if(PW >= Lim_MotorPW){ 
				
		//���io�͵�ƽ
		MotorDown();
				
		}
	  
		PW++;//100us=0.1ms����һ��
		if(PW == 100)PW=0;//10ms
			
		MotorPW++;//100us=0.1ms����һ��
		if(MotorPW == 200)MotorPW = 0;//20ms
	
	}
}

 /*******************************************************************************
* Function Name  : UsartSignalToLED_model
* Description    : ���ݵõ��Ĵ����źž����Ǽ��ŵ�������������һ���̶�ֵ���档
* Input          : UsrtSignal�û������ź� OpenCloseFlag�����ǿ���ʼ�صĶ�����PWM���裩
* Output         : ���ŵ���
* Return         : None
* Attention		 : OpenCloseFlag = 1  ��Ӧ�����ź�
*******************************************************************************/
void UsartSignalToLED_model(char UsrtSignal,short OpenCloseFlag){
	
	switch(UsrtSignal){
	
		case 'z' :{if(OpenCloseFlag)ProgramLedOpen();//�����õģ�����ָʾ��
						else ProgramLedClose();}break;
		case 'a' :{if(OpenCloseFlag);//Ҫ�޸ĳ����еƹر�
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
* Description    : �û������ź�ת������ָ��Lim_PW
* Input          : UsrtSignal
* Output         : ����ָ��(pwmռ����)
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



/*-------------------��������Ͱ--------------------------------------
RedLed1Close();//����ƹ�
		ProgramLedOpen();//����ָʾLED��
		BlueLedOpen();//���ƿ�
		PBout(6)=1;
		PBout(10)=1;
		_750LedClose();
		delay_ms(1500);
		RedLed1Open();//����ƿ�
		ProgramLedClose();//����ָʾLED��
		_750LedOpen();
		BlueLedClose();//���ƹ�
		PBout(6)=0;
		PBout(10)=0;
		delay_ms(1500);
 
 
---------------------��������Ͱ--------------------------------------*/






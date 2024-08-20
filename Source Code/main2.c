#include "stm32f10x.h"                  
#include "stm32f10x_rcc.h"              
#include "stm32f10x_gpio.h"             
#include "stm32f10x_tim.h" 
#include "stm32f10x_usart.h"  
#include "stm32f10x_exti.h"             
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
/*prototype*/
void PWM_StandardLibrary(void);
void PWM_Pulse( int Pulse);
void USART_config(void);
void SendByte(uint8_t byte);
void USART_Send_String( uint8_t *Data, uint8_t Len);
void TIM4_config(void );
void Delay_ms(uint32_t ms);
void Delay_us( uint32_t us);
void EXIT0_Encoder_Init(void);
void EXTI0_IRQHandler(void);
void USART3_IRQHandler(void);
void TIM_Sample(uint8_t ms);
void TIM3_IRQHandler(void);

/*var*/
volatile float cnt = 0;
volatile short vong = 0;
uint8_t string[] = "tung\n\r";
int var = 100;
uint8_t msg[30];
int sprintf(char *str, const char *format,...);
double goc_xoay;
char buffer[10];
uint8_t pos = 0;
int mv;
/*main function*/
int main(void){
	PWM_StandardLibrary();
	TIM4_config();
	EXIT0_Encoder_Init();
	USART_config();
	
	while(1){
		goc_xoay = (cnt*360)/220;
		sprintf(msg, "%f\n\r", goc_xoay);
		USART_Send_String(msg, sizeof(msg)); 
		
	}
}
/**/

/* PA0 PWWM, PA2: In1, PA3: In2*/
void PWM_StandardLibrary(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef timerInit;
	TIM_OCInitTypeDef pwmInit;
	/**/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/**/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/**/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/**/
	timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	timerInit.TIM_Period = 100-1;
	timerInit.TIM_Prescaler = 36-1;
	TIM_TimeBaseInit(TIM2, &timerInit);
	TIM_Cmd(TIM2, ENABLE);
	
	/* Ch1 with duty 10 % */
	pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	pwmInit.TIM_OCPolarity = TIM_OCPolarity_High;
	pwmInit.TIM_Pulse = 0;
	pwmInit.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM2, &pwmInit);
}
/**/
void PWM_Pulse( int Pulse){
	TIM2->CCR1 =(uint8_t) Pulse;
}
/**/
/**/
/*UART, PB10 : Tx, PB11 : Rx*/
void USART_config(void){
/**/
	/* khoi tao cac bien cau truc*/
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	/**/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	/**/
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	/* config PB 10 for Tx*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	/**/
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	/**/
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	
	/* cho phep USART hoat dong*/
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	/*USART2 config*/
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = 1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_InitStruct);USART_Cmd(USART3,ENABLE);
	/**/
	USART_Cmd(USART3, ENABLE);
	}
void USART3_IRQHandler(void){
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		USART_ClearFlag(USART3,USART_FLAG_RXNE);
		buffer[pos++] =(uint8_t) USART_ReceiveData(USART3);
		if(buffer[pos] == '>'){
			pos =0;
			mv = atoi(buffer);
		}
	}
}
void SendByte(uint8_t byte){

	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==0);
	USART_SendData(USART3, byte);
	}
void USART_Send_String( uint8_t *Data, uint8_t Len){
	uint8_t i;
	for(i = 0; i<Len; i++){
		
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==0);
		USART_SendData(USART3, *Data);
		Data++;
		}
		
	}
/*Time Base Init*/
void TIM4_config(void ){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM4, ENABLE);
}

void Delay_ms(uint32_t ms){
	
	while(ms>0){
		TIM4->CNT = 0U;
		while(TIM4->CNT <1000);
		ms--;
		}
	}
void Delay_us( uint32_t us){
	TIM4_config();
	while(us>0){
		TIM4->CNT = 0U;
		while(TIM4->CNT <1);
		us--;
		}
	}
/**/
/* EXITO PB0, PA7 */
void EXIT0_Encoder_Init(void){
	/**/
	/**/
	GPIO_InitTypeDef My_GPIO;
	EXTI_InitTypeDef EXTI_InitStruc;
	NVIC_InitTypeDef NVIC_InitStruct;
	/**/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);
	/**/
	My_GPIO.GPIO_Pin = GPIO_Pin_0;
	My_GPIO.GPIO_Mode = GPIO_Mode_IPU;
	My_GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &My_GPIO);
	/**/
	My_GPIO.GPIO_Pin = GPIO_Pin_7;
	My_GPIO.GPIO_Mode = GPIO_Mode_IPU;
	My_GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &My_GPIO);
	/**/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
	/**/
	EXTI_InitStruc.EXTI_Line = EXTI_Line0;
	EXTI_InitStruc.EXTI_LineCmd = ENABLE;
	EXTI_InitStruc.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruc.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruc);
	/**/
	/*clear pending bit*/
	EXTI_ClearITPendingBit(EXTI_Line0);
	EXTI_ClearITPendingBit(EXTI_Line1);
	/*NVIC config*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	/**/
	
	}
void EXTI0_IRQHandler(void){
	if((EXTI_GetITStatus(EXTI_Line0))!=RESET){
		/* clear pending bit cho lan tiep theo*/
		EXTI_ClearITPendingBit(EXTI_Line0);
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) ==0){
			if((vong==0)&& (cnt ==0)){
				cnt = 0;
			}
			cnt++;
		}
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) !=0){
			if((vong==0) && (cnt ==0) ){
				cnt = 0;
			}
			cnt--;
		}
		if(cnt>220){
			vong++;
			cnt = 0;
		}
		if(cnt<-220){
			vong--;
			cnt = 0;
		}
	}
}

/* TIM3 intercrupt Init*/
void TIM_Sample(uint8_t ms){
	/**/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	/**/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 720 - 1; /*100KHz*/
	TIM_TimeBaseInitStruct.TIM_Period = ms*100; /*ms = Period/100KHz*/
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	/**/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	/**/
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	TIM_Cmd(TIM3, ENABLE);
	}
/**/
void TIM3_IRQHandler(void){
		if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == 1){
			TIM_ClearFlag(TIM3, TIM_FLAG_Update);
			
			}
		}
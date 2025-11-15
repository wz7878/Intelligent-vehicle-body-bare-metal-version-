#include "stm32f4xx.h"
#include "sys.h"
#include "usart.h"
#include "esp8266.h"
#include "delay.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>

static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static NVIC_InitTypeDef   		NVIC_InitStructure;

void tim3_init(void)
{
	//使能tim3的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//配置tim3的分频值、计数值
	//tim3硬件时钟=84MHz/8400=10000Hz，就是进行10000次计数，就是1秒时间的到达

	TIM_TimeBaseStructure.TIM_Period = 10000-1; //计数值0 -> 9999就是1秒时间的到达
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;	//预分频值8400
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//时钟分频，当前是没有的，不需要进行配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//配置tim3的中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
}


void TIM3_IRQHandler(void)
{
	static uint32_t cnt=0;

	//检测标志位
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		if(cnt!=g_esp8266_rx_cnt)
		{
			cnt=g_esp8266_rx_cnt;
		}
		else if(cnt && (cnt == g_esp8266_rx_cnt))
		{
			g_esp8266_rx_end=1;
			cnt=0;
		}
		
		//清空标志位
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}

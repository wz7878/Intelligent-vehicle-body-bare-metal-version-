#include "pwm.h"

/*
	PE5	TIM9_CH1
	APB2 84MHZ
*/
void Pwm_Steering_TIM9_CH1_PE5_Init()
{
	GPIO_InitTypeDef 		GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef 		TIM_OCInitStruct;
	
	
	// （2）使能定时器9和GPIOE时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE); // TIM9挂载在APB2总线
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	// （3）初始化IO口为复用功能输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	// （4）GPIOE5复用映射到定时器9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
	
	// （5）初始化定时器时基
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84-1;  // 84MHz / 84 = 1MHz（计数周期1μs）
	TIM_TimeBaseInitStruct.TIM_Period = 20000-1; // 周期20000×1μs=20ms
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseInitStruct);
	
	// （6）初始化输出比较参数（TIM9_CH1）
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0; // 初始占空比
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM9, &TIM_OCInitStruct); // 通道1初始化（对应PE5）
	
	// （7）使能预装载寄存器
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable); 
	
	// （8）使能自动重装载预装载
	TIM_ARRPreloadConfig(TIM9, ENABLE);
	
	// （9）使能定时器
	TIM_Cmd(TIM9, ENABLE);
}

#include "sr04.h"

/*****************************************
引脚说明
连接超声波， PA2 – TX TRIG（触发信号）  PA3 – RX ECHO（回响信号）
PA2  TX 做为普通输出
PA3  RX 做为普通输入

*******************************************/
void Sr04_Init(void)
{

	//定义结构体变量
	GPIO_InitTypeDef	GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;

	
	//打开GPIOA组时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	//1、能定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_2;		//引脚2
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP; 	//推挽输出
	GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_UP; 	//上拉
	GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_25MHz; //25MHZ
	GPIO_Init(GPIOA, &GPIO_InitStruct);		
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3;		//引脚3
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;		//输入模式
	GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_UP; 	//上拉

	GPIO_Init(GPIOA, &GPIO_InitStruct);			
	
	TIM_TimeBaseInitStruct.TIM_Prescaler	= 84-1; // 84分频 84MHZ/84 = 1MHZ
	TIM_TimeBaseInitStruct.TIM_Period	= 50000-1;					
	TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up;	// 向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;	// 分频因子
	//2、初始化定时器，配置ARR,PSC
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	
			
	//5,不使能定时器
	TIM_Cmd(TIM5, DISABLE);
}

u16 Get_Sr04_Value(void)
{
	u16 temp, dis, t = 0;
	
	//PA2输出低电平
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	delay_us(8);
	//PA2输出高电平
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	delay_us(20);
	//PA2输出低电平
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	
	//设置定时器的CNT为0  TIM3->CNT = 0
	TIM5->CNT = 0;
	
	//PA3等待高电平到来，参考按键松开代码 while( 读电平  );
	while( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == Bit_RESET)
	{
		t++;
		
		delay_us(5);
		
		if(t >= 1000) //5000us未监测到高电平，则返回
		{
			printf("T:%d\r\n", t);	
			return 0;		
		}
	}
 	
	
	//使能定时器开始计数
	TIM_Cmd(TIM5, ENABLE);
	
	t = 0;
	//PA3等待低电平到来，while( 读电平  );
	while( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == Bit_SET)
	{
		t++;
		
		delay_us(10);
		
		if(t >= 3000) //3000us未监测到低电平，则返回
		{
			printf("T:%d\r\n", t);	
			return 0;
		}
	}
	

	//获取定时器CNT值，从而得到高电平持续时间    
	temp = TIM5->CNT;

	//printf("temp:%d\r\n", temp);
	
	//关闭定时器
	TIM_Cmd(TIM5, DISABLE);
	//通过公式计算出超声波测量距离

	dis = temp/58;
	
	return dis;
}

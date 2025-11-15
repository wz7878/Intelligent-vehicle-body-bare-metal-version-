#include <delay.h>


uint32_t tick_us = 0;
uint32_t tick_ms = 0;

//适配其他M3和M4的开发板
//初始化毫秒，微妙延时的计数值
//设置系统定时器的时钟源
//SysCLK： 当前使用该延时函数的开发板的系统时钟频率  --  STM32F407ZET6--168MHz---传递168
void delay_init( uint16_t SysClk)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//系统定时器的频率设置为AHB/8= 168MHz/8=21MHz
	tick_us = SysClk / 8;									//当前使用开发板频率为168，168/8=21 -- 1us所需计数值
	tick_ms = tick_us * 1000;								//1ms所需的计数值
}

//微妙延时
//nus: 需要延时多少微妙  
void delay_us( uint32_t nus)
{
	uint32_t temp;		//读取CTRL寄存器的值， 判断定时器是否被意外关闭，
	
	SysTick->CTRL &=  ~SysTick_CTRL_ENABLE_Msk; 			//关闭系统定时器  SysTick_CTRL_ENABLE_Msk宏定义: SysTick->CTRL &=  ~(1<<0)
	SysTick->LOAD =   tick_us * nus - 1;					//设置计数值
	SysTick->VAL = 0; 										//清空当前值寄存器, 清空定时完成的标志位
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk; 				//打开系统定时器  SysTick->CTRL = (1<<0)， 只设置系统定时器的启动开关，时钟源不变， 不启用中断
	
	do
	{
		temp = SysTick->CTRL;
	}while(( temp & SysTick_CTRL_ENABLE_Msk) && !(temp & 0x01<<16));
	//( temp & SysTick_CTRL_ENABLE_Msk) --> 判断定时器是否在其他地方被意外关闭 ， 真-没有被意外关闭，正常运行   假-被意外关闭 
	//(temp & 0x01<<16)                 --> 判断定时器是否定时完毕
	
	SysTick->CTRL &=  ~SysTick_CTRL_ENABLE_Msk; 			//关闭系统定时器  SysTick_CTRL_ENABLE_Msk宏定义: SysTick->CTRL &=  ~(1<<0)
}

//毫妙延时
//nms: 需要延时多少毫妙  

void delay_ms( uint32_t nms)
{
	//nms : 在21MHz频率下最大值： 798ms 
	
	uint32_t temp;		//读取CTRL寄存器的值， 判断定时器是否被意外关闭，
	
	SysTick->CTRL &=  ~SysTick_CTRL_ENABLE_Msk; 			//关闭系统定时器  SysTick_CTRL_ENABLE_Msk宏定义: SysTick->CTRL &=  ~(1<<0)
	SysTick->LOAD =   tick_ms * nms - 1;					//设置计数值
	SysTick->VAL = 0; 										//清空当前值寄存器, 清空定时完成的标志位
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk; 				//打开系统定时器  SysTick->CTRL = (1<<0)， 只设置系统定时器的启动开关，时钟源不变， 不启用中断
	
	do
	{
		temp = SysTick->CTRL;
	}while(( temp & SysTick_CTRL_ENABLE_Msk) && !(temp & 0x01<<16));
	//( temp & SysTick_CTRL_ENABLE_Msk) --> 判断定时器是否在其他地方被意外关闭 ， 真-没有被意外关闭，正常运行   假-被意外关闭 
	//(temp & 0x01<<16)                 --> 判断定时器是否定时完毕
	
	SysTick->CTRL &=  ~SysTick_CTRL_ENABLE_Msk; 			//关闭系统定时器  SysTick_CTRL_ENABLE_Msk宏定义: SysTick->CTRL &=  ~(1<<0)
	
	
#if 0 
	
	//可以延时任意时间
	while(nms--)
	{
		SysTick->CTRL &=  ~SysTick_CTRL_ENABLE_Msk; 			//关闭系统定时器  SysTick_CTRL_ENABLE_Msk宏定义: SysTick->CTRL &=  ~(1<<0)
		SysTick->LOAD =   tick_ms - 1;					//设置计数值
		SysTick->VAL = 0; 										//清空当前值寄存器, 清空定时完成的标志位
		SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk; 				//打开系统定时器  SysTick->CTRL = (1<<0)， 只设置系统定时器的启动开关，时钟源不变， 不启用中断
		
		do
		{
			temp = SysTick->CTRL;
		}while(( temp & SysTick_CTRL_ENABLE_Msk) && !(temp & 0x01<<16));
		//( temp & SysTick_CTRL_ENABLE_Msk) --> 判断定时器是否在其他地方被意外关闭 ， 真-没有被意外关闭，正常运行   假-被意外关闭 
		//(temp & 0x01<<16)                 --> 判断定时器是否定时完毕
	
		SysTick->CTRL &=  ~SysTick_CTRL_ENABLE_Msk; 			//关闭系统定时器  SysTick_CTRL_ENABLE_Msk宏定义: SysTick->CTRL &=  ~(1<<0)
	}
#endif
}

void delay_s( uint32_t ns)
{
	
	while(ns--)
	{
		delay_ms(500);
		delay_ms(500);
	}
}

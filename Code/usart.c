//#include <usart.h>


//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;

//int fputc(int ch, FILE *f) {
//	
//	USART_SendData(USART1, ch);		//通过串口1发送ch字节数据
//	while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);	//没有发送完毕，则等待发送完毕
//	USART_ClearFlag(USART1, USART_FLAG_TXE);	

//	return(ch);
//}

//char g_usart1_rx_buff[USART1_RX_SIZE]={0};
//int  g_usart1_rx_index=0;

//void usart1_init( uint32_t USART_BaudRate)
//{
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;

//	//使能串口1的时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

//	//使能GPIOA的时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	
//	//设置PA9和PA10引脚为复用功能-串口
//	GPIO_PinAFConfig( GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
//	GPIO_PinAFConfig( GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
//	
//	//设置引脚为复用模式
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	//串口参数的初始化
//	USART_InitStructure.USART_BaudRate 	 = USART_BaudRate;					//波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;				//数据位长度-8bit
//	USART_InitStructure.USART_StopBits   = USART_StopBits_1;				//停止位 - 1bit
//	USART_InitStructure.USART_Parity     = USART_Parity_No;					//校验位 -- 不使用校验功能
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//硬件控制流 -- 不使用
//	USART_InitStructure.USART_Mode 		 = USART_Mode_Rx | USART_Mode_Tx;	//串口启动 接受与发送
//	USART_Init(USART1, &USART_InitStructure);
//	
//	//配置中断  -- 接受中断
//	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE);
//	
//	/* Enable the USARTx Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel 		= USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd 	= ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//	//使能串口
//	USART_Cmd(USART1, ENABLE);
//}


//void usart_sendchar(char ch)
//{
//	USART_SendData(USART1, ch);		//通过串口1发送ch字节数据
//	while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);	//没有发送完毕，则等待发送完毕
//	USART_ClearFlag(USART1, USART_FLAG_TXE);	//清除 USARTx 的待处理标志
//}

//void usart_sendstr(char *str)
//{
//	char *p = str;
//	
//	while(*p != '\0')
//	{
//		usart_sendchar(*p);
//		p++;
//	}
//}


////串口1的中断服务函数
//void USART1_IRQHandler(void)
//{
//	//判断是否接收到数据 -- 每接收1个字节数据，触发一次中断
//	if(USART_GetITStatus( USART1, USART_IT_RXNE) == SET)
//	{
//		char ch = USART_ReceiveData(USART1);	//接受串口1的数据
////		usart_sendchar(ch);		//把接受到数据，返回到发送方，用于检测是否接受到数据
//		
//		if(g_usart1_rx_index <= USART1_RX_SIZE-1)
//		{
//			g_usart1_rx_buff[ g_usart1_rx_index] = ch;		//拼接会字符串
//			g_usart1_rx_index++;		//自动指向下一项
//		}
//		else	//自定义  字符串接受完毕的标志
//		{
//			g_usart1_rx_buff[ g_usart1_rx_index] = '\0';	//接受'#'结束符，写入\0表示字符串结束
//			g_usart1_rx_index = 0;		//清空接受位置， 用于下一次的接受
//			
//			//处理数据
//			usart_sendstr( g_usart1_rx_buff);
//		}
//		
//		
//		USART_ClearITPendingBit( USART1, USART_IT_RXNE);
//	}
//}


#include "stm32f4xx.h"
#include "sys.h"
#include "esp8266.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static USART_InitTypeDef   		USART_InitStructure;
static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef   		NVIC_InitStructure;

#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{
	USART_SendData(USART1,ch);
		
	//等待数据发送成功
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);


	return ch;
}

void _sys_exit(int return_code) {

}



void usart1_init(uint32_t baud)
{
	//使能端口A硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//使能串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	//配置PA9、PA10为复用功能引脚
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//将PA9、PA10连接到USART1的硬件
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	
	//配置USART1的相关参数：波特率、数据位、校验位
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//允许串口发送和接收数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	//使能串口接收到数据触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口1工作
	USART_Cmd(USART1,ENABLE);
}

void usart3_init(uint32_t baud)
{
	//使能端口B硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//使能串口3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//配置PB10、PB11为复用功能引脚
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将PB10、PB11连接到USART3的硬件
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	
	//配置USART1的相关参数：波特率、数据位、校验位
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//允许串口发送和接收数据
	USART_Init(USART3, &USART_InitStructure);
	
	
	//使能串口接收到数据触发中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口3工作
	USART_Cmd(USART3,ENABLE);
}

void usart3_send_str(char *str)
{
	char *p = str;
	
	while(*p!='\0')
	{
		USART_SendData(USART3,*p);
		
		p++;
		
		//等待数据发送成功
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	}
}


void usart3_send_bytes(uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	while(len--)
	{
		USART_SendData(USART3,*p);
		
		p++;
		
		//等待数据发送成功
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	}
}


void USART1_IRQHandler(void)
{
	uint8_t d=0;
	
	//检测是否接收到数据
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		d=USART_ReceiveData(USART1);
		
#if EN_DEBUG_ESP8266		
		//将接收到的数据发给串口3
		USART_SendData(USART3,d);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
#endif		
		//清空标志位，可以响应新的中断请求
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



void USART3_IRQHandler(void)
{
	uint8_t d=0;
	
	//检测是否接收到数据
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		d=USART_ReceiveData(USART3);
		
		g_esp8266_rx_buf[g_esp8266_rx_cnt++]=d;
		
		
		if(g_esp8266_rx_cnt >= sizeof g_esp8266_rx_buf)
		{
			g_esp8266_rx_end=1;
		}

#if EN_DEBUG_ESP8266		
		//将接收到的数据返发给PC
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
#endif		
		//清空标志位，可以响应新的中断请求
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

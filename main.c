#include <stm32f4xx.h>
#include <delay.h>
#include <pwm.h>
#include <stdio.h>
#include <usart.h>
#include <sr04.h>
#include <oled.h>
#include <iic.h>
#include <dht11.h>
#include <beep.h>
#include <fan.h>
#include <tire.h>
#include "esp8266.h"
#include "tim.h"
#include <string.h>
#include <stdlib.h>
#include "sys.h"

#define AP_SSID "wz"
#define AP_PASS "11111111"


int32_t net_init(char* ssid,char* pswd);
int32_t net_connect_server(void);
int32_t net_disconnect_server(void);

static uint32_t g_connect_server_sta=0;

char dht11_data[5] = {0};

void Led_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIOF的设备时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	// 打开GPIOE的设备时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//初始化GPIO设备参数 -- PF9 
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;		//9,10号引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;		//输出模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;	//反转速度越快，功耗越高， 信号不稳定
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出模式
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//浮空
	GPIO_Init( GPIOF, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init( GPIOE, &GPIO_InitStruct);
	
	GPIO_SetBits( GPIOF, GPIO_Pin_9);
	GPIO_SetBits( GPIOF, GPIO_Pin_10);
	GPIO_SetBits( GPIOE, GPIO_Pin_13);
	GPIO_SetBits( GPIOE, GPIO_Pin_14);
}

void Exit_PA0_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIOA的设备时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;		//0号引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;		//输入模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;	//反转速度越快，功耗越高， 信号不稳定
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出模式
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;		//上拉
	GPIO_Init( GPIOA, &GPIO_InitStruct);
}

void Exit_PE2_PE3_PE4_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开GPIOA的设备时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;		//0号引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;		//输入模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;	//反转速度越快，功耗越高， 信号不稳定
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出模式
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;		//上拉
	GPIO_Init( GPIOE, &GPIO_InitStruct);
}

void sr04_oled_tire()
{
	u16 dis;
	float tire_pressure;
	char buf[30]={0};
	OLED_Clear();
	dis = Get_Sr04_Value();
	
	//将数据发送到服务端
	sprintf(buf,"distance:%d cm",dis);
	esp8266_send_str(buf);
	
	if(dis<10)	//超声波警报
	{
//		beep_on();  delay_ms(100);
//		beep_off(); delay_ms(100);	
//		beep_on();  delay_ms(100);
//		beep_off(); delay_ms(100);
		GPIO_ResetBits( GPIOF, GPIO_Pin_9);  delay_ms(100);
		GPIO_SetBits( GPIOF, GPIO_Pin_9); delay_ms(100);	
		GPIO_ResetBits( GPIOF, GPIO_Pin_9);  delay_ms(100);
		GPIO_SetBits( GPIOF, GPIO_Pin_9); delay_ms(100);
	}
	OLED_ShowTest(0,0,"distance:");
	OLED_ShowNum(72,0,dis,3,16);
	printf("dis:%d\r\n", dis);
	
	OLED_ShowTest(0, 2, "hum:");        // 第二行（页2-3）
	OLED_ShowNum(30, 2, dht11_data[0], 2, 16);  // 与上一行同页
	OLED_ShowChar(50, 2,'.',16);
	OLED_ShowNum(55, 2, dht11_data[1], 2, 16);
	
	OLED_ShowTest(0, 4, "tem:");        // 第三行（页4-5）
	OLED_ShowNum(30, 4, dht11_data[2], 2, 16);  // 与上一行同页
	OLED_ShowChar(50, 4,'.',16);
	OLED_ShowNum(55, 4, dht11_data[3], 2, 16);
	
	tire_pressure = Press_GetTirePressure(); // 读取当前胎压
	
	//将胎压发送到客户端
	sprintf(buf,"tire_pressure:%.1f kPa",tire_pressure);
	esp8266_send_str(buf);
	
	if(tire_pressure>=100 || tire_pressure<=50)	//胎压警报
	{
		GPIO_ResetBits( GPIOF, GPIO_Pin_10);  delay_ms(100);
		GPIO_SetBits( GPIOF, GPIO_Pin_10); delay_ms(100);	
		GPIO_ResetBits( GPIOF, GPIO_Pin_10);  delay_ms(100);
		GPIO_SetBits( GPIOF, GPIO_Pin_10); delay_ms(100);
	}
	printf("Tire Pressure: %.1f kPa\r\n", tire_pressure);
	OLED_ShowTest(0, 6, "tire:");
	OLED_ShowNum(50,6,tire_pressure,3,16);
}

//0-180度舵机 --> 0-90-180度控制函数
void Server_180_Angle(void)
{
	TIM_SetCompare1(TIM9,1000);//45度  1ms
	sr04_oled_tire();
	delay_s(1);
	 
	TIM_SetCompare1(TIM9,1500);//90度  1.5ms
	sr04_oled_tire();
	delay_s(1);
	
	TIM_SetCompare1(TIM9,2000);//135度  2ms
	sr04_oled_tire();
	delay_s(1);
	
	TIM_SetCompare1(TIM9,1500);//90度  1.5ms
	sr04_oled_tire();
	delay_s(1);
}

int main(void)
{
	int ret = 0;
//	beep_init();	//嗡鸣器 PF8
	delay_init(168);
	Led_Init();		//LED
	usart1_init(115200);
	iic_init();				//初始化IIC的IO引脚 PB8、9
	OLED_Init();			//初始化OLED  
	OLED_Clear(); 
	printf("T345436\r\n");
	Sr04_Init();	//超声波	PA2、3，TIM3
	
	Exit_PA0_Init();	//
	Exit_PE2_PE3_PE4_Init();
	
	Pwm_Steering_TIM9_CH1_PE5_Init();	//PE5,TIM9
	
	// 初始化系统时钟
//    SystemInit();
    // 初始化PWM：频率=1kHz（84MHz/(83+1)/(999+1)=1kHz）
    Fan_PWM_Init(999, 83);	//PB6，TIM4
	
	Press_ADC_Init();
	
	esp8266_init();
	
	delay_ms(100);
	
	printf("This is esp8266 connect ap by Teacher.Wen\r\n");
	
	//NVIC分组 抢占优先级占2bit 响应优先级占2bit
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	int flag=0;
	int rt;
	char buf[30]={0};
	while(1)
	{
		if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET)    //按下
        {
            delay_ms(15);    //消抖，消除误差，防止错误判断为按下
          
            if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == Bit_RESET)
            {
				rt = net_init(AP_SSID,AP_PASS);
				
				if(rt == 0)
				{
					printf("连接WIFI成功！\r\n");
					GPIO_ResetBits( GPIOE, GPIO_Pin_13);  delay_ms(100);
					GPIO_SetBits( GPIOE, GPIO_Pin_13); delay_ms(100);	
					GPIO_ResetBits( GPIOE, GPIO_Pin_13);  delay_ms(100);
					GPIO_SetBits( GPIOE, GPIO_Pin_13); delay_ms(100);
				}
				else
				{
					printf("连接WIFI失败！\r\n");
				}
				
				/* 连接服务器 */
				rt = net_connect_server();
				
				if(rt == 0)
				{
					printf("[OK] net connect server success\r\n");
					GPIO_ResetBits( GPIOE, GPIO_Pin_14);  delay_ms(100);
					GPIO_SetBits( GPIOE, GPIO_Pin_14); delay_ms(100);
					GPIO_ResetBits( GPIOE, GPIO_Pin_14);  delay_ms(100);
					GPIO_SetBits( GPIOE, GPIO_Pin_14); delay_ms(100);
					
					flag=1;
				}
				else
				{
					printf("[error] net connect server fail\r\n");
				}	
//				flag=1;
            }
        }
		
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==Bit_RESET)
		{
			delay_ms(15);
			if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==Bit_RESET)
			{
				rt = net_disconnect_server();
			
				if(rt == 0)
				{
					printf("[OK] net disconnect server success\r\n");
					PFout(10)=1;
				}
				else
				{
					printf("[error] net disconnect server fail\r\n");
				}
				rt =esp8266_disconnect_ap();	
				if(rt == 0)
				{
					printf("[OK] net disconnect ap success\r\n");	
					PFout(9)=1;
				}
				else
				{
					printf("[error] net disconnect ap fail\r\n");
				}	
				
				OLED_Clear();
				flag=0;
			}
		}
		
		if(flag==1)
		{
			ret = Dht11_ReadData(dht11_data);
			if( ret == 0)
			{
				printf("湿度:%d.%d  温度:%d.%d\r\n", dht11_data[0],dht11_data[1],dht11_data[2],dht11_data[3]);
				sprintf(buf,"hum:%d.%d ,tem:%d.%d",dht11_data[0],dht11_data[1],dht11_data[2],dht11_data[3]);
				//将温湿度发送到服务端
				esp8266_send_str(buf);
				if(dht11_data[2]>=25)
				{
					Fan_SetDuty(500);
				}
				else
				{
					Fan_SetDuty(0);
				}
			}
			else{
				printf("读取温湿度数据失败: %d", ret);
			}
			
			Server_180_Angle();
			TIM_SetCompare1(TIM9,50);
		}
	}
}



/* 网络初始化 */
int32_t net_init(char* ssid,char* pswd)
{
	int32_t rt;
	
	/* esp8266退出透传模式，才能执行AT指令 */
	esp8266_exit_transparent_transmission();
	
	/* 进行自检测试 */
	rt=esp8266_self_test();
	
	if(rt == 0)
	{
		printf("[OK] esp8266 self test success\r\n");	
	}
	else
	{
		printf("[error] esp8266 self test  fail\r\n");
		
		return -1;
	}		
	
	/* 连接热点 */
	rt = esp8266_connect_ap(ssid,pswd);
	
	if(rt == 0)
	{
		printf("[OK] esp8266 connect ap %s success\r\n",ssid);

	}
	else
	{
		printf("[error] esp8266 connect ap %s fail\r\n",ssid);
				
		return -3;
	}	
	
	return 0;

}

/* 连接服务器 */
int32_t net_connect_server(void)
{
	int32_t rt;
	
	/* 连接到网络服务器 */
	esp8266_send_str("AT+CIPSTART=\"TCP\",\"192.168.137.54\",9999\r\n");
	
	rt = esp8266_find_str_in_rx_packet("OK",5000);
	
	if(rt)
		return rt;
	
	/* 进入透传模式 */
	rt=esp8266_entry_transparent_transmission();
	if(rt)
		return rt;
	
	return 0;
}



/* 断开服务器 */
int32_t net_disconnect_server(void)
{
	/* 退出透传模式 */
	esp8266_exit_transparent_transmission();
	
	/* 断开服务器连接 */
	return esp8266_disconnect_server();
}


#include <dht11.h>

/*
	Dht11_Pin_SetMode(GPIO_Mode_Out);		//设置为输出模式
	Dht11_Pin_SetMode(GPIO_Mode_IN);		//设置为输入模式
*/
void Dht11_Pin_SetMode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	
	//打开GPIOF的设备时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 ;		//9号引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode;			//模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;	//反转速度越快，功耗越高， 信号不稳定
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;		//推挽输出模式
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//浮空
	GPIO_Init( GPIOG, &GPIO_InitStruct);
	
	if( GPIO_Mode == GPIO_Mode_OUT)	
		DHT11_OUT = 1;			//初始化时，设置PG9引脚为高电平， DHT11空闲状态
}

int Dht11_Start()
{
	int t = 0;
	
	//设置输出模式
	Dht11_Pin_SetMode( GPIO_Mode_OUT);

	DHT11_OUT = 0;		//输出至少18ms的低电平
	delay_ms(20);
	DHT11_OUT = 1;		//输出20-40us的高电平, 等待响应信号输出
	delay_us(30);
	
	//设置输入模式 -- 接受响应信号/40bit数据
	Dht11_Pin_SetMode( GPIO_Mode_IN);
	
	t=0;
	while( DHT11_IN == 1)	//等待响应信号低电平的出现， 因为发6送起始信号后，DHT11并不是立刻输出响应信号
	{
		t++;
		delay_us(1);
		if( t > 4000 )  //整个通信过程(40bit数据+响应信号)持续4ms，如果4ms内都没有出现响应信号， 则证明通信失败
			return -1;
	}
	
	t=0;
	while( DHT11_IN == 0)	//等待响应信号的80us低电平结束
	{
		t++;
		delay_us(1);	
		if( t > 90)			//如果低电平持续时间超过90us,认为通信失败
			return -2;
	}
	
	t=0;
	while( DHT11_IN == 1)	//等待响应信号中的80us高电平结束
	{
		t++;
		delay_us(1);	
		if( t > 90)			//如果高电平持续时间超过90us,认为通信失败
			return -3;
	}
	
	return 0;
}

int Dht11_GetByte()
{
	int index = 0;
	int d = 0;
	int t = 0;	//超时
	
	for( index = 0; index < 8; index++)
	{
		t=0;
		while( DHT11_IN == 0)	//等待1bit数据的50us时隙结束
		{
			t++;
			delay_us(1);	
			if( t > 60)			//如果高电平持续时间超过60us,认为通信失败
				return -4;
		}
		
		delay_us(50);			//50us时隙结束后，进入高电平， 高电平的持续时间区分'1'/'0','1'-70us '0'-26~28us
								//延时50us的时间， '0'--持续时间已经结束,进入到下一个bit的50us低电平时隙 -- 低电平
								//				   '1'--持续时间还没有结束， 剩余20us高电平  -- 高电平
								//读取 电平状态 即可分出'1'/'0'
		
		t=0;
		if( DHT11_IN == 1)		//高电平，则证明'1'
		{
			d |= (1<<(7-index));	//将bit数据保存到变量，接受， MSB
			
			while( DHT11_IN == 1)	//等待'1'剩余的20us高电平结束， 才可以开始接受下一个bit的数据
			{
				t++;
				delay_us(1);
				if( t > 30)
					return -5;
			}
		}
	}
	
	return d;
}

int Dht11_ReadData( char *buff)
{
	int i = 0;
	int ret = 0;
	
	ret = Dht11_Start();
	if( ret)  return ret; 
	
	for( i = 0; i < 5; i++)		//接受40bit的数据
	{
		buff[i] = Dht11_GetByte();			//写入到数组中
		if( buff[i] < 0)					//如果读取字节数据时，返回-4/-5,则证明数据读取错误
			return buff[i];					//返回对应的错误码
	}
	
	if( buff[4] != (buff[0]+buff[1]+buff[2]+buff[3]))		//校验， 如果正确buff[4]校验和 等于 buff[0]~buff[3]相加
	{	
		return -6;
	}
	
	return 0;
}

#include <iic.h>

//对软件IIC的通信引脚进行初始化
void iic_init(void)
{
	GPIO_InitTypeDef 		GPIO_InitStruct 		= {0};
	
	//打开GPIOB的设备时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9 ;	//9号引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;				//输出模式
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;			//反转速度越快，功耗越高， 信号不稳定
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;				//开漏输出模式
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				//上拉模式
	GPIO_Init( GPIOB, &GPIO_InitStruct);

	//初始化完毕后，IIC总线处于空闲状态
	SCL = 1;
	SDA_OUT = 1;
}

void iic_sda_mode( GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef 		GPIO_InitStruct 		= {0};
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 ;	
	GPIO_InitStruct.GPIO_Mode  = mode;				
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;				
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;				
	GPIO_Init( GPIOB, &GPIO_InitStruct);
}

//IIC的开始信号
void iic_start(void)
{
	iic_sda_mode( GPIO_Mode_OUT);	//设置SDA引脚为输出模式
	
	SCL = 1;
	SDA_OUT = 1;		//空闲状态
	delay_us(5);
	SDA_OUT = 0;		//SDA引脚电平从高电平->低电平, 起始信号
	delay_us(5);
	SCL = 0;			//SCL进入低电平状态， 做数据通信的准备
}


//IIC的停止信号
void iic_stop(void)
{
	iic_sda_mode( GPIO_Mode_OUT);	//设置SDA引脚为输出模式
	
	SCL = 0;			
	SDA_OUT = 0;	//发送结束信号前的准备
	delay_us(5);
	SCL = 1;		//SCL进入高电平状态
	delay_us(5);	
	SDA_OUT = 1;	//SDA在SCL处于高电平状态时,从低电平->高电平， 停止信号
}

//IIC发送一个字节的数据
void iic_send_byte( uint8_t byte)
{
	int8_t index = 0;
	
	iic_sda_mode( GPIO_Mode_OUT);	//设置SDA引脚为输出模式
	
	SCL = 0;
		
	for( index = 0; index < 8; index++)
	{
		if( byte&(1<<(7-index)))	//真-对应bit为1
			SDA_OUT = 1;			//准备逻辑'1'
		else
			SDA_OUT = 0;			//准备逻辑'0'
		delay_us(5);
		
		SCL = 1;					//拉高时钟线， 将SDA钳位在对应电平
		delay_us(5);
		SCL = 0;					//拉低时钟线， 进入下一个bit准备
	}

}


//IIC发送一个应答信号
//ack - 1:无效应答  0:有效应答
void iic_send_ack( uint8_t ack)
{
	iic_sda_mode( GPIO_Mode_OUT);	//设置SDA引脚为输出模式
	
	SCL = 0;
		
	if( ack)					//真-高电平-无效应答
		SDA_OUT = 1;			//准备逻辑'1'
	else						//假-低电平-有效应答
		SDA_OUT = 0;			//准备逻辑'0'
	delay_us(5);
	
	SCL = 1;					//拉高时钟线， 将SDA钳位在对应电平
	delay_us(5);
	SCL = 0;					//拉低时钟线， 进入下一个bit准备
}

//接收一个字节的数据
uint8_t iic_recv_byte( void)
{
	uint8_t data = 0;
	int8_t index = 0;
	iic_sda_mode( GPIO_Mode_IN);	//设置SDA引脚为输入模式
	
	SCL = 0;			//设置低电平，通知从机设备准备好需要传输的数据
	
	for( index = 0; index < 8; index++)
	{
		delay_us(5);	//低电平延时，等待从机准备数据
		
		SCL = 1;		//将SCL拉高， 从机就会通过SDA引脚发送数据
		delay_us(5);
		
		if( SDA_IN)		//真-SDA输入高电平
			data |= 1<<(7-index);
			
		SCL = 0;		//接收数据完毕，将SCL设置为低电平， 通知对端设备准备数据
	}	
	
	return data;
}

//接收一个字节的数据
//返回值- 0:接收到有效应答  1:接收到无效应答
uint8_t iic_recv_ack( void)
{
	uint8_t ack = 0;

	iic_sda_mode( GPIO_Mode_IN);	//设置SDA引脚为输入模式

	SCL = 0;			//设置低电平，通知从机设备准备好需要传输的数据

	delay_us(5);	//低电平延时，等待从机准备数据
	
	SCL = 1;		//将SCL拉高， 从机就会通过SDA引脚发送数据
	delay_us(5);
	
	if( SDA_IN)		//真-SDA输入高电平
		ack = 1;
		
	SCL = 0;		//接收数据完毕，将SCL设置为低电平， 通知对端设备准备数据

	return ack;
}






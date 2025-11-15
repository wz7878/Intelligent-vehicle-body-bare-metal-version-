#ifndef __IIC_H__
#define __IIC_H__

#include <stm32f4xx.h>
#include <sys.h>
#include <delay.h>

#define  SCL  		PBout(8)
#define  SDA_OUT    PBout(9)		//SDA输出电平
#define  SDA_IN     PBin(9)			//SDA输入电平

extern void iic_init(void);
extern void iic_start(void);
extern void iic_stop(void);
extern void iic_send_byte( uint8_t byte);
extern void iic_send_ack( uint8_t ack);
extern uint8_t iic_recv_byte( void);
extern uint8_t iic_recv_ack( void);

#endif

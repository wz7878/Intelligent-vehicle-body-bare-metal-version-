#ifndef __DELAY_H__
#define __DELAY_H__

#include <stm32f4xx.h>

extern void delay_init( uint16_t SysClk);
extern void delay_us( uint32_t nus);
extern void delay_ms( uint32_t nms);
extern void delay_s( uint32_t ns);
	

#endif

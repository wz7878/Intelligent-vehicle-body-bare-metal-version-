//#ifndef __USART_H__
//#define __USART_H__

//#include <stm32f4xx.h>
//#include <stdio.h>

//#define USART1_RX_SIZE   512

//extern void usart1_init( uint32_t USART_BaudRate);
//extern void usart_sendchar(char ch);
//extern void usart_sendstr(char *str);

//#endif

#ifndef __USART_H__
#define __USART_H__


extern void usart1_init(uint32_t baud);
extern void usart3_init(uint32_t baud);

extern void usart3_send_str(char *str);
extern void usart3_send_bytes(uint8_t *buf,uint32_t len);




#endif


#ifndef __DHT11_H__
#define __DHT11_H__

#include <stm32f4xx.h>
#include <sys.h>
#include <delay.h>

#define DHT11_OUT  PGout(9)
#define DHT11_IN   PGin(9)

extern int Dht11_ReadData( char *buff);

#endif

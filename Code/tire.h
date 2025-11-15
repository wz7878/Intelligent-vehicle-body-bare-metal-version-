#ifndef __TIRE_ADC_H__
#define __TIRE_ADC_H__

#include <stm32f4xx.h>
#include "delay.h"
#include "usart.h"

void Press_ADC_Init(void);
float Press_GetTirePressure(void);

#endif


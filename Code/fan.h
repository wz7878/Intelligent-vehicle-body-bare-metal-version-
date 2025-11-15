#ifndef __FAN_PWM_H
#define __FAN_PWM_H

#include "stm32f4xx.h"
#include "delay.h"

void Fan_PWM_Init(uint16_t arr, uint16_t psc);
void Fan_SetDuty(uint16_t duty);

#endif

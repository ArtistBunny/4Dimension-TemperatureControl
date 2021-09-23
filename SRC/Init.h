
#ifndef __INIT__H__
#define __INIT__H__

#include "stm32f10x.h"

void Init_LED_GPIO(void);
void Init_Key_GPIO(void);
void Init_Exti(void);
void Init_Timer(void);
void Init_DAC(void);
void Init_ADC(void);
void KEY_GPIO_config(void);//°´¼ü³õÊ¼»¯
void LED_GPIO_Init(void);

void GPIOLED_Configuration(void);
void Tim_Configuration(void);
void NVIC_init(void);
void AD_init(void);
void KEY_GPIO_config(void);
void LED_GPIO_Init(void);




#endif

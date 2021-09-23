
#include "Init.h"
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

void GPIOLED_Configuration(void)   
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE); 

	//LEDS 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;  	 //位码GPIO端口配置
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;   //段码GPIO端口配置
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;  	 //加热电阻端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}

void Tim_Configuration(void)  //定时器配置
{
   
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); //TIM1时钟使能
	
  //时基单元配置
  TIM_TimeBaseStructure.TIM_Period = 2000;	 //5MS定时时间常数
  TIM_TimeBaseStructure.TIM_Prescaler = 71; //71+1 72M进行72分频 即1M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	
  TIM_InternalClockConfig(TIM1); //此句可屏蔽，默认选为内部时钟  
	
  //TIM_ITConfig(TIM1,TIM_FLAG_Update,ENABLE);  //使能定时更新中断
  //TIM_Cmd(TIM1, ENABLE);  //使能定时器
	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1000;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); 
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
	
void NVIC_init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1定时中断通道使能
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  NVIC_Init(&NVIC_InitStructure);  
}


void KEY_GPIO_config(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_12|GPIO_Pin_13; //按键端口配置 PD3
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO口速度为50MHz
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;//内部上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


//4个LED IO端口初始化
void LED_GPIO_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE); //使能PA,PC端口时钟
												   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_6| GPIO_Pin_7;	//LED4 LED3 LED2端口配置
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
  	GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				//LED1端口配置
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		GPIO_SetBits(GPIOC, GPIO_Pin_13);//PC13置高,LED4灭
}




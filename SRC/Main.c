
#include "Init.h"
#include "Fun.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"
#include "table.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
unsigned int ADC_ConvertedValue;
unsigned char Data_Buffer[4]={1,2,3,4}; //����ܻ��� 
unsigned int Duan[19]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x76,0x40,0}; //����ܶ���

unsigned char kuaisu=0;  //�������ٴ���
unsigned char V_mode=0; //0-4
unsigned int canshu[5]={0,450,110,50,10};//���Ŵ�10��
//ģʽ0���¶�ʵʱֵ��ʾ��ǰ1λ�������ʾģʽ����2λ��ʾʵʱ�¶�ֵ����ȷ��0.1C
//ģʽ1��Ŀ���¶��趨��35-65�ȣ�
//ģʽ2��PID�����е�P��������ģʽ��0.0-50.0��
//ģʽ3��PID�����е�I��������ģʽ��0.0-50.0��
//ģʽ4��PID�����е�D��������ģʽ��0.0-50.0��
unsigned int maxcanshu[5]={0,650,500,500,500};  //�������ڷ�Χ���ֵ 
unsigned int mincanshu[5]={0,350,1,1,1};     //�������ڷ�Χ��Сֵ 


unsigned char ADflag=0; 
unsigned char PIDflag=0;   //PID�����־
unsigned char Disp_flag=0; //�����������ʾ����
unsigned int pwmcount=0;  //PWMռ�ձ�
unsigned int HARDPWM=0;  //PWMռ�ձ�

//ʹ��DMA ADC�Զ�����ɼ�NTC�¶�ͨ��ֵ
void AD_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
   ADC_InitTypeDef   ADC_InitStructure;
	 DMA_InitTypeDef   DMA_InitStructure;
	
	/* GPIOC Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

  /* pc4:pointer,PC0 vbus                            */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize         = 1;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel1,  ENABLE);        /* Enable DMA Channel1                */	  
  	
	/* ADC1 Configuration (ADC1CLK = 18 MHz) -----------------------------------*/
  ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						  
  ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel       = 1;		//ADת��ͨ����
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 Regular Channel1 Configuration                                      */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_55Cycles5);	  
  
	ADC_Cmd(ADC1, ENABLE); 
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1)); 
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_DMACmd(ADC1, ENABLE);             /* Enable ADC1's DMA interface        */
  
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);/* Start ADC1 Software Conversion     */
}

	

unsigned char pid_val_mid=0;
void PIDcompute()
{
  /*--------------------------------------------------------------------
�����趨���ɼ�ֵ���м���PID���ڣ������PID����VAL����MID��ֵ
--------------------------------------------------------------------*/
static int SumError=0,PrevError=0,LastError=0;
int dError=0,Error=0;
double j=0.0,i;


    Error =canshu[1]-canshu[0];     
		if(Error>10){pid_val_mid=250;return;}
		else if(Error+10<0){pid_val_mid=0;return;}
	  SumError +=Error;                   
    dError=Error-LastError;
    PrevError=LastError;
    LastError=Error;

	  i=canshu[2];
		j=Error*i;
		i=canshu[3];
		j=j+SumError*i;
		i=canshu[4];
		j=j+dError*i;


	if(j>0) j=j/10; //PID�����Ŵ���10��������Ҫ��С10��
	
	if(j>250)pid_val_mid=250;//ȫ��
	else if(j<0)pid_val_mid=0;//ȫ��
	else pid_val_mid=j; //����ֵ
}

void Buffer_fresh()
{  		
		Data_Buffer[0]=V_mode;
	  Data_Buffer[1]=canshu[V_mode]/100%10;
	  Data_Buffer[2]=canshu[V_mode]/10%10;
		Data_Buffer[3]=canshu[V_mode]%10;
}

//NTC�¶Ȳ�����	
unsigned int TempSampleComu(void)
{	
	float t;
	unsigned int dat,max,min,mid,da,j;
	
	      t=ADC_ConvertedValue;
				t=t/4096;
				t=t*3300;
				t=t/(5-t/1000);
				dat=t*10;
			
	      da=dat;
		    max=97;	min=0;	
		
				while(1)	
				{
				mid=(max+min)/2;
				if(Table[mid]<da)		max=mid;
				else 					min=mid;
				if((max-min)<=1)		break;
				}	
      
				if(max==min)da=min*10;
				else 
				{
					j=(Table[min]-Table[max])/10;
					j=(Table[min]-da)/j;
					da=j;
					da=10*min+da; //�ɼ����¶ȷŴ���10��
				 }
				
return da;		 
}


//TIM1��ʱ�����жϺ���
void TIM1_UP_IRQHandler(void)
{
  static unsigned char Bit=0;	
 static unsigned int count=0,count1=0,keytime=0;//���ʱ�������� 

  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		TIM_SetCompare3(TIM1,pid_val_mid*2000/4000);
		
     Bit++;	
     if(Bit>=4)Bit=0;
     GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_10); //�ض�����λ��		 
					
		 if(Bit==2||Bit==0)GPIO_Write(GPIOE,Duan[Data_Buffer[Bit]]|0x80); 
		 else GPIO_Write(GPIOE,Duan[Data_Buffer[Bit]]); 
		 switch(Bit)				 //��λ��
			{
			 case 0: GPIO_ResetBits(GPIOB,GPIO_Pin_10);break; 
			 case 1: GPIO_ResetBits(GPIOB,GPIO_Pin_11);break;
			 case 2: GPIO_ResetBits(GPIOB,GPIO_Pin_12);break;
			 case 3: GPIO_ResetBits(GPIOB,GPIO_Pin_13);break;
		  }		 
			
			
		 count++;
		 if(count>=100)				//200ms���²ɼ�һ���¶�,����һ��PID
		 {
				count=0;   
				ADflag=1;	    
		 }  
		 
		 
		 count1++;
		 if(count1>=400)				//800ms��һ��PID
		 {
				count1=0;   
				PIDflag=1;	    
		 }  

     //PD6��ΪUP�� PD12��ΪDOWN��
		 if((GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==Bit_RESET||GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)==Bit_RESET)&&V_mode!=0)
			{
				if(++keytime>60)
				{
					keytime=0;
					if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)==Bit_RESET)
						{canshu[V_mode]++;if(canshu[V_mode]>maxcanshu[V_mode])canshu[V_mode]=mincanshu[V_mode];Disp_flag=1;}
					else if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)==Bit_RESET)
						{if(canshu[V_mode]==mincanshu[V_mode])canshu[V_mode]=maxcanshu[V_mode];canshu[V_mode]--;Disp_flag=1;}
				}
			}
		else {keytime=0;}  
  }
}


int main(void)
{		    	
	unsigned char  x=0,y=0;
	unsigned int k;
	
	KEY_GPIO_config();
	GPIOLED_Configuration();
	Tim_Configuration();
	NVIC_init();
	AD_init();	
	while(1)
	{
		if(Disp_flag==1)
	  	{ Disp_flag=0;Buffer_fresh();}
	  if(ADflag==1)
			{	canshu[0]=TempSampleComu();ADflag=0;
				if(V_mode==0)Disp_flag=1;}
	  
		
		if(PIDflag==1)
			{	PIDflag=0;PIDcompute();}
			
			
			
			
	
		 if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)==Bit_SET)x=0;  //K4������Ϊģʽ��
		 else if(x==0)
		 {
			  for(k=0;k<10000;k++);
				if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)==Bit_RESET)
				{
					x=1;
					V_mode++;Disp_flag=1;
					if(V_mode>4)V_mode=0;
				}
			} 

			if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)==Bit_SET)y=0; //K2������Ϊȷ����
			else if(y==0)
		 {
				y=1;
				Disp_flag=1;
				V_mode=0;
			 } 
	 
	  pwmcount++;
	  if(pwmcount>250)
	    {
		  pwmcount=0;
		 }
	  
	  if(pwmcount>=pid_val_mid)    GPIO_ResetBits(GPIOB,GPIO_Pin_15);//�����ȵ���
	  else   GPIO_SetBits(GPIOB,GPIO_Pin_15);//����		
	}
}



#include "Fun.h"


//��ʱʱ�亯��  US
void delay_us(unsigned int time)
{
	unsigned char i;
 
  while(time-->0)
     {for(i=0;i<8;i++);}
}

//��ʱʱ�亯��  MS
void delay_ms(unsigned int time)
{ 
  while(time-->0)delay_us(1000);
}







#include "Fun.h"


//延时时间函数  US
void delay_us(unsigned int time)
{
	unsigned char i;
 
  while(time-->0)
     {for(i=0;i<8;i++);}
}

//延时时间函数  MS
void delay_ms(unsigned int time)
{ 
  while(time-->0)delay_us(1000);
}






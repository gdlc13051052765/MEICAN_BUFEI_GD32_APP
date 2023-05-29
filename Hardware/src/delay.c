#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "delay.h"

void delay_ns(uint32_t ns)
{
	uint32_t i;
	for(i=0;i<ns;i++)
	{
		__nop();
		__nop();
		__nop();
	}
}

void mDelay(int i)
{
	delay_ns(i*1000);
}

void delay_us(uint32_t us)
{
	uint32_t i = 0;
	for(i=0;i<us;i++){
	uint8_t a = 10;
	while(a--);
	}
}


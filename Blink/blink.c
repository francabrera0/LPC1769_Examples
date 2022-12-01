#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include "gpiohandler.h"

void clock(void);
void delay(unsigned int);

int main(void) {
	SystemInit();

	gpioConfig(PORT0,0,NEITHER,OUTPUT);
	gpioConfig(PORT0,1,NEITHER,OUTPUT);
	gpioConfig(PORT0,22,NEITHER,OUTPUT);
	gpioConfig(PORT2,12,PULLDOWN,INPUT);
	gpioWrite(PORT0,0,LOW);
	gpioWrite(PORT0,1,HIGH);
	gpioWrite(PORT0,22,0);
	gpioWrite(PORT0,22,1);

	int s = 0;
	uint8_t randomByte = 0x55;
	uint32_t out1 = randomByte<<1 | 0x1;
	uint32_t out2 = 0x256;
	uint32_t sequence = 0;
	uint32_t actual = 0;
	int counter = out1;

	while(1){
		delay(1000);
		s = getState(PORT2, 12);

		if(s == 1){
			sequence = out2;
			if(actual!=sequence){
				counter = 0;
				actual=sequence;
			}
		}
		else{
			sequence = out1;
			if(actual!=sequence){
				counter = 0;
				actual=sequence;
			}
		}

		clock();
		gpioWrite(PORT0,0,(sequence>>counter & 0x01));
		counter = (counter==10) ? 0:counter+1;

	}
}

void delay(unsigned int count){
	for (unsigned int i=0; i<count; i++)
		for(unsigned int j=0; j<5000; j++);
}

void clock(void){
	gpioWrite(PORT0,22,0);
	delay(100);
	gpioWrite(PORT0,22,1);
}

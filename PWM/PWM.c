/*
===============================================================================
 Name        : PWM.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

void configGPIO(void);
void configINT(void);

uint8_t dutyCicle = 1;
uint8_t control = 0;

int main(void) {
	configGPIO();
	configINT();

	while(1){
		if(control<dutyCicle)
			LPC_GPIO0->FIOCLR2 |= (1<<6);
		else
			LPC_GPIO0->FIOSET2 |= (1<<6);
	}

    return 0 ;
}

void configGPIO(void){
	LPC_PINCON->PINSEL1 &= ~(3<<12);
	LPC_GPIO0->FIODIR2 |= (1<<6);
	LPC_GPIO0->FIOCLR |= (1<<22);

	LPC_PINCON->PINSEL4 |= (1<<20);
	LPC_PINCON->PINMODE4 &= ~(3<<20);
	LPC_GPIO2->FIODIR1 &= ~(1<<2);
}

void configINT(){
	LPC_SC->EXTINT |= 1;
	LPC_SC->EXTMODE |= 1;
	LPC_SC->EXTPOLAR &= ~(1);
	NVIC_SetPriority(EINT0_IRQn,1);
	NVIC_EnableIRQ(EINT0_IRQn);

	SysTick->LOAD = ((SystemCoreClock/1000)-1);
	SysTick->CTRL = (1) | (1<<1) | (1<<2);
	SysTick->VAL = 0;
	NVIC_SetPriority(SysTick_IRQn,0);
}

void EINT0_IRQHandler(){
	dutyCicle++;
	if(dutyCicle>10)
		dutyCicle = 1;
	LPC_SC->EXTINT |= 1;
}

void SysTick_Handler(){
	control++;
	if(control>=10)
		control=0;
	SysTick->CTRL &= SysTick->CTRL;
}


/*
===============================================================================
 Name        : WaveFormTimers.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#define SET_PR 50000

void configGPIO(void);
void configEint(void);
void configTimer0(void);

uint8_t state = 0;

int periods[3] = {499, 259, 166};


int main(void) {
	configGPIO();
	configEint();
	configTimer0();

	while(1){
		uint32_t out = LPC_TIM0->CR1 * 10;
		LPC_GPIO0->FIOPIN0 = out>>10;
	}
    return 0 ;
}

void configGPIO(void){
	LPC_PINCON->PINSEL0 = 0;
	LPC_GPIO0->FIODIR0 = 0xFF;
}

void configEint(void){
	LPC_PINCON->PINSEL4 |= (1<<20);
	LPC_SC->EXTINT |= (1);
	LPC_SC->EXTMODE |= (1);
	LPC_SC->EXTPOLAR |= (1);
	NVIC_SetPriority(EINT0_IRQn, 1);
	NVIC_EnableIRQ(EINT0_IRQn);
}

/*
 * Cálculo de tiempos
 * PR=0 --> TC se incrementa en 1 cada pulso de CCLK = 10[ns]
 * 		f=100[KHz] --> T=10[us] --> Ttogle=5[us] --> MATCH = 499 (499+1*10[ns]=5[us])
 * 		f=200[KHz] --> T=5[us] --> Ttogle=2.5[us] --> MATCH = 249 (249+1*10[ns]=2.5[us])
 * 		f=300[KHz] --> T=3.33[us] --> ttogle=1.66[us] --> MATCH = 166 (166+1*10[ns]=1.67[us])
 */
void configTimer0(void){
	LPC_PINCON->PINSEL3 |= (3<<24); //Pin 1.28 configurado como MAT0.0
	LPC_PINCON->PINSEL3 |= (3<<20); //Pin 1.26 configurado como CAP0.0
	LPC_PINCON->PINSEL3 |= (3<<22);

	LPC_SC->PCONP |= (1 << 1);
	LPC_SC->PCLKSEL0 |= (1 << 2);
	LPC_SC->PCLKSEL0 &= ~(1<<3);

	LPC_TIM0->PR = SET_PR;
	LPC_TIM0->MR0 = periods[0];
	LPC_TIM0->MCR = 2; //Timer0 reinicia al hacer match
	LPC_TIM0->TCR = 3; //Reset y habilitación
	LPC_TIM0->TCR &= ~2; //Sale del estado de reset
	LPC_TIM0->IR |= 0x3F; //Limpia banderas de INT
	LPC_TIM0->EMR |= 1;
	LPC_TIM0->EMR |= (3<<4); //MAT0.0 modo toggle

	LPC_TIM0->CCR = 6;	//CAP0.0 interrumpe por flanco descendente
	LPC_TIM0->CCR |= (1<<4); //CAP0.1 flanco descendente NO INTERRUMPE

}

void EINT0_IRQHandler(void){
	state++;
	if(state>2) state = 0;
	NVIC_EnableIRQ(TIMER0_IRQn); //Habilita la int para el CAP0.0
	LPC_SC->EXTINT |= 1;
}


void TIMER0_IRQHandler(void){
	LPC_TIM0->TCR |= 2;
	LPC_TIM0->MR0 = periods[state];
	LPC_TIM0->TCR &= ~(2);
	NVIC_DisableIRQ(TIMER0_IRQn);
	LPC_TIM0->IR |= 1<<4;
}




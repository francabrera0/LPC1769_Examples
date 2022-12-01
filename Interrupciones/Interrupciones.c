
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include <cr_section_macros.h>
#include "gpioHandler.h"

void delay(uint32_t time);
void configIntGpio(void);

uint8_t ctrl;

int main(void) {
	uint32_t time;

	/*
	 * Configuración de puertos
	 *  P0.22 -> LedRojo integrado -> Salida
	 *  P0.15 -> Entrada con pulldown
	 *  P0.16 -> Entrada con pulldown
	 */
	gpioConfig(PORT0,22,NEITHER,OUTPUT);
	gpioConfig(PORT0,15,PULLDOWN,INPUT);
	gpioConfig(PORT0,16,PULLDOWN,INPUT);

	/*
	 * Puerto 0.1 configurado como salida en 1 para poder tener 3.3v para generar las
	 * interrupciones
	 */
	gpioConfig(PORT0,1,NEITHER,OUTPUT);
	gpioWrite(PORT0,1,HIGH);

	/*
	 * Configuración de los parámetros de interrupciones
	 */
	configIntGpio();

	while(1){
		time = (ctrl%2) ? 1000000:4000000;

		gpioWrite(PORT0,22,LOW);
		delay(time);
		gpioWrite(PORT0,22,HIGH);
		delay(time);
	}
}

/*
 * Rutina de servicio de la interrupción de gpio
 */
void EINT3_IRQHandler(void){
	delay(10000);
	if(LPC_GPIOINT->IO0IntStatR & (1<<15)){
		ctrl=0;
		LPC_GPIOINT->IO0IntClr |= (1<<15);
	}
	else if(LPC_GPIOINT->IO0IntStatR & (1<<16)){
		ctrl=1;
		LPC_GPIOINT->IO0IntClr |= (1<<16);
	}
}

/*
 * Hibilitación y configuración de las interrupciones
 *
 * Las interrupciones del gpio comparten vector con la EINT3
 */
void configIntGpio(void){
	LPC_GPIOINT->IO0IntEnR |= (1<<15); //Habilito int por flanco subida p0.15
	LPC_GPIOINT->IO0IntClr |= (1<<15); //Limpia banderas de int
	LPC_GPIOINT->IO0IntEnR |= (1<<16); //Habilito int por flanco subida p0.16
	LPC_GPIOINT->IO0IntClr |= (1<<16); //Limpia banderas de int
	NVIC_EnableIRQ(EINT3_IRQn); //Habilito las interrupciones externas
	                            //ES LO MISMO QUE PONER ISER0 |=(1<<21|1<<18) ; QUE ESTA EN LA TABLA 52 Y ES EINT3
}

void delay (uint32_t time){
	uint32_t i;
	for(i=0; i<time; i++){};
}



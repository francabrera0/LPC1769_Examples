#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void portsConfig(void);
void intConfig(void);

int ctrl=0; //Para dividir el tiempo y poder observarlo
int counter=0; //Para sacar la secuencia, es el valor con el que se hace shift a la salida
uint8_t out = 0xaf; //Secuencia de salida (00001010)

int main(void) {
	portsConfig();
	intConfig();

	while(1){}; //Trabaja todo por interrupciones

    return 0 ;
}


void portsConfig(void){
	LPC_PINCON->PINSEL1 &= ~(3<<12); //P0.22 como gpio
	LPC_GPIO0->FIODIR2 |= (1<<6); //P0.22 como salida

	LPC_PINCON->PINSEL4 |= (1<<24); // P2.12 como EINT2 (pin 25:24 = 0b01)
	LPC_PINCON->PINMODE4 &= ~(3<<24); // Entrada con pullUp
	LPC_GPIO2->FIODIR1 &= ~(1<<4); //P2.12 como entrada
}

void intConfig(void){
	LPC_SC->EXTMODE |= (1<<2); //Interrupción por flanco
	LPC_SC->EXTPOLAR &= ~(1<<2); //Flanco descendente
	LPC_SC->EXTINT |= (1<<2); //Limpio las banderas
	NVIC_SetPriority(EINT2_IRQn,0); //Prioridad 0
	NVIC_EnableIRQ(EINT2_IRQn); //Habilitación de interrupciones

	if(SysTick_Config(SystemCoreClock/100)) //Uso la función del cmsis, si no se puede hacer como está abajo
		while(1);							//En caso de error se queda ahí la ejecución del programa

	/*SysTick->LOAD = ((SystemCoreClock/100)-1);
	SysTick->CTRL = (1) | (1<<1) | (1<<2);
	SysTick->VAL = 0;
	NVIC_SetPriority(SysTick_IRQn,5);*/
}

void EINT2_IRQHandler(void){
	static int state; //Variable que representa el estado del sistema, 0 --> Systick no interrumpe
	if(state){
		LPC_GPIO0->FIOSET |= (1<<22); //Deja el led apagado
		SysTick->CTRL &= ~(1<<1); //Deshabilita la int de systick
		state = 0; //Cambio de estado
	}
	else{
		SysTick->CTRL |= (1<<1); //Habilita la int Systick
		state = 1; //Cambia de estado
	}

	LPC_SC->EXTINT |= (1<<2); //Limpia el flag de la int
}


void SysTick_Handler(void){
	ctrl++; //Solamente sirve para dividir el tiempo y poder observar el led
	if(ctrl==10){
		LPC_GPIO0->FIOPIN = ((out>>counter & 0x01)<<22); //Pone en el led el estado de la secuencia
		counter = (counter==7) ? 0:counter+1;
		ctrl=0;
	}
	SysTick->CTRL &= SysTick->CTRL; //Limpia el flag de int
}

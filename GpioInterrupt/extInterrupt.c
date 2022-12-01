
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include <cr_section_macros.h>


int retardo (unsigned int  time);
uint8_t ctrl = 0;

int main(void) {

	LPC_PINCON->PINSEL1 &= ~(3<<12);// BITS 12-13 GPIO para P0.22
	LPC_GPIO0->FIODIR |= (1<<22);//P0.22 como salida.

	LPC_PINCON->PINSEL4 &= (~(2 << 20))|(~(2<<26)); //Bits 21-20 puestos en 0b01 (2)
                                                    //Bits 27-26 puestos en 0b01 (2)
                                                   // FUNCION DE GPIO P2.10 y P2.13.//Para EINT0 e EINT3.

    LPC_GPIO2->FIODIR1 &= (~(1<<2))|(~(1<<5));//Habilito P2.10 y P2.13 como inputs.//Mirar que es DIR1 y no DIR.

    //P2.10 como INT0
    LPC_SC->EXTINT      |= 1;   //Limpia bandera de interrupci�n
    LPC_SC->EXTMODE     |= 1; //Selecciona interrupcion por flanco
    LPC_SC->EXTPOLAR    |= 1; //Interrumpe cuando el flanco es de subida//table 12.
    //P2.13 como INT3
    LPC_SC->EXTINT      |= 1<<3;   //Limpia bandera de interrupci�n
    LPC_SC->EXTMODE     |= 1<<3; //Selecciona interrupcion por flanco
    LPC_SC->EXTPOLAR    &= ~(1<<3); //Interrumpe cuando el flanco es de bajada.

    NVIC_SetPriority(EINT3_IRQn,1);//Seteo prioridad 1 para EINT3 ->Setea IP register ver tablas de 62-70/
    NVIC_SetPriority(EINT0_IRQn,2);//Seteo prioridad 2 para EINT0

    //habilito la interrupciones externas
    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn); //ES LO MISMO QUE PONER ISER0 |=(1<<21|1<<18) ; QUE ESTA EN LA TABLA 52 Y ES EINT3
                                //habilito interrupción externa 3.
                                // Interruption Set Enable Register.ISERx
    unsigned int time = 1000000;

    while(1){
    	time = (ctrl%2) ? 1000000:4000000;
        LPC_GPIO0->FIOSET |= 1<<22;
        retardo(time);
        LPC_GPIO0->FIOCLR |= 1<<22;
        retardo(time);
    }
}


int retardo (unsigned int time){
    unsigned int i;
    for(i=0 ; i<time;i++);
    return 0;
}

void EINT0_IRQHandler(void) {
	ctrl = 0;
	LPC_SC->EXTINT      |= 1;
}

void EINT3_IRQHandler(void) {
    ctrl = 1;
	LPC_SC->EXTINT      |= 1<<3;
}
}



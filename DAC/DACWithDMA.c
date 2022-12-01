/*
===============================================================================
 Name        : DMA1.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"


#endif

#include <cr_section_macros.h>

#define DMA_SIZE 20


void confPin(void);
void confDac(void);
void configDMA2(void); //M2P
void fillPrt1(void);

GPDMA_LLI_Type LLI1;
uint32_t *prt1=(uint32_t *) 0x2007C000;
uint32_t periods[3] = {25000, 12500, 6250};
uint8_t ctrl = 0;

int main(void) {

	*(prt1) =  1023<<6;
	*(prt1+1) = 640<<6;
	*(prt1+2) = 640<<6;
	*(prt1+3) = 512<<6;
	*(prt1+4) = 512<<6;
	*(prt1+5) = 384<<6;
	*(prt1+6) = 384<<6;
	*(prt1+7) = 256<<6;
	*(prt1+8) = 384<<6;
	*(prt1+9) = 128<<6;
	*(prt1+10) = 0<<6;
	*(prt1+11) = 128<<6;
	*(prt1+12) = 0<<6;
	*(prt1+13) = 256<<6;
	*(prt1+14) = 0<<6;
	*(prt1+15) = 384<<6;
	*(prt1+16) = 0<<6;
	*(prt1+17) = 512<<6;
	*(prt1+18) = 0<<6;
	*(prt1+19) = 640<<6;


	confPin();
	confDac();
	configDMA2();
	GPDMA_ChannelCmd(0,ENABLE);
	while(1) {
    }
    return 0 ;
}


void configDMA2(void){
	NVIC_DisableIRQ(DMA_IRQn);

	LLI1.SrcAddr=(uint32_t) prt1;
	LLI1.DstAddr= (uint32_t) &(LPC_DAC->DACR);
	LLI1.NextLLI=(uint32_t)&LLI1 ;
	LLI1.Control= DMA_SIZE
				| (2<<18)
				| (2<<21)
				| (1<<26);


	GPDMA_Init();
	GPDMA_Channel_CFG_Type  GPDMACfg;
	GPDMACfg.ChannelNum = 0;
	GPDMACfg.SrcMemAddr = (uint32_t)LLI1.SrcAddr;
	GPDMACfg.DstMemAddr = 0;
	GPDMACfg.TransferSize = DMA_SIZE;
	GPDMACfg.TransferWidth = 0;
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	GPDMACfg.SrcConn = 0;
	GPDMACfg.DstConn = GPDMA_CONN_DAC;
	GPDMACfg.DMALLI = (uint32_t)&LLI1;
	GPDMA_Setup(&GPDMACfg);
	return;
}

void confPin(void){
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(2, 1<<20, 0);

	EXTI_InitTypeDef extiCfg;
	extiCfg.EXTI_Line = EXTI_EINT0;
	extiCfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	extiCfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	EXTI_Config(&extiCfg);
	EXTI_Init();
	NVIC_EnableIRQ(EINT0_IRQn);

	return;
}

void confDac(void){
	DAC_CONVERTER_CFG_Type DAC_ConverterConfigStruct;
	DAC_ConverterConfigStruct.CNT_ENA =SET;
	DAC_ConverterConfigStruct.DMA_ENA = SET;
	DAC_Init(LPC_DAC);
	DAC_SetDMATimeOut(LPC_DAC,periods[0]);
	DAC_ConfigDAConverterControl(LPC_DAC, &DAC_ConverterConfigStruct);
	return;
}

void EINT0_IRQHandler(void){
	ctrl++;
	if(ctrl>=3)
		ctrl = 0;
	GPDMA_ChannelCmd(0, DISABLE);
	DAC_SetDMATimeOut(LPC_DAC, periods[ctrl]);
	GPDMA_ChannelCmd(0, ENABLE);
	EXTI_ClearEXTIFlag(EXTI_EINT0);
}


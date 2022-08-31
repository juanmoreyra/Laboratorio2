/******************************************************************************
 * @file           : main.c
 * @author         : Juan Moreyra
 * @brief          : Main program body
 ******************************************************************************/


/************************************MACROS***********************************/
#include <stdint.h>
#define ITER_ESPERA 400000
//Macros para CONFIGURAR CLK (función configura_led())
#define RCC_BASE 0x40021000 									/** p. 50 **/
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18)) 	/** p. 112 **/
#define RCC_APB2ENR_IOPCEN (1<<4) 								/** p. 114 **/
//Macros para CONFIGURAR CLK (función configura_timer())
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x1C)) 	/** p. 112 **/
#define RCC_APB1ENR_TIM3EN (1<<1)								/** p. 117**/
//Macros para función configura_led()
#define GPIOC_BASE 0x40011000 									/** p 51 **/
#define GPIOC_CRH (*(volatile uint32_t *)(GPIOC_BASE + 0x04)) 	/** p. 172 **/
#define GPIOC_CRH_MODE13_0 (1<<20)
#define GPIOC_CRH_MODE13_1 (1<<21)
#define GPIOC_CRH_CNF13_0 (1<<22)
#define GPIOC_CRH_CNF13_1 (1<<23)
//Macro para prende_led() y apaga_led()
#define GPIOC_ODR *((volatile uint32_t *)(GPIOC_BASE + 0x0C)) /** p. 173 **/
//Macros para configurar timer
#define TIM3_BASE 0x40000400								  /** p. 52 **/
#define TIM3_CR1 (*(volatile uint32_t *)(TIM3_BASE + 0x00))  /** p. 404 **/
#define TIM3_CR1_CEN 1
#define TIM3_PSC (*(volatile uint32_t *)(TIM3_BASE + 0x28))  /** p. 418 **/
#define TIM3_PSC_0 1
#define TIM3_PSC_1 (1<<1)
#define TIM3_PSC_2 (1<<2)
#define TIM3_CNT (*(volatile uint32_t *)(TIM3_BASE + 0x24))  /** p. 418**/
#define TIM3_ARR (*(volatile uint32_t *)(TIM3_BASE + 0x2C))  /** p. 419**/
#define TIM3_EGR (*(volatile uint32_t *)(TIM3_BASE + 0x14))	 /** p. 412**/
/***********************************FUNCIONES*********************************/
void configura_led();
void prende_led();
void apaga_led();
void espera(uint32_t);
void espera_ms(uint32_t);
void configura_timer();
/*************************************MAIN************************************/
volatile uint32_t ms_pausa = 500;
int main(void)
{
	configura_led();
	configura_timer();
	while(1){
		prende_led();
		espera_ms(ms_pausa);
		apaga_led();
		espera_ms(ms_pausa);
	}

}


void espera(uint32_t iter){
	uint32_t b =0;
	while(b++ < iter);
}

void configura_led(){
	// habilita el clock del GPIOC
	RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	// establece PC13 como salida push-pull / 2MHz max
	GPIOC_CRH &= ~(GPIOC_CRH_MODE13_0|GPIOC_CRH_MODE13_1|GPIOC_CRH_CNF13_0|GPIOC_CRH_CNF13_1); //pongo los 4 bits en cero
	GPIOC_CRH |= GPIOC_CRH_MODE13_1;
}

void prende_led(){
	// prende el LED - poniendo a GND PC13
	GPIOC_ODR &= ~(1 << 13);
}

void apaga_led(){
	// apaga el LED - poniendo a VDD PC13
	GPIOC_ODR |= (1 << 13);
}

void configura_timer(){
	//habilita el clock del TIM3
	RCC_APB1ENR |= RCC_APB1ENR_TIM3EN;

	//Verifico que el registro TIM3_CR1 esté cargado por completo con ceros
	TIM3_CR1 = 0x0000;



	//configuracion del prescaller
	TIM3_PSC = 7;						//TIM3_PSC |= (TIM3_PSC_0|TIM3_PSC_1|TIM3_PSC_2);

	//Contador en 0
	TIM3_CNT = 0;

	//Auto-Reload al máximo
	TIM3_ARR = 0xFFFF;

	//Genero un evento de update por software
	TIM3_EGR |= (1<<0);

	//TIMER ENABLE
		TIM3_CR1 |=TIM3_CR1_CEN;
}

void espera_ms(uint32_t ms){
	for(; ms>0 ; ms--){
		TIM3_CNT = 0;
		//Genero un evento de update por software
		//TIM3_EGR |= (1<<0);
		while(TIM3_CNT < 1000);
	}
}

/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                     */
/************************************************************************/

// LEDs
#define LED_PIO            PIOC                 // periferico que controla o LED
#define LED_PIO_ID         ID_PIOC  // ID do periférico PIOC (controla LED)    
#define LED_PIO_IDX        8                    // ID do LED no PIO
#define LED_PIO_IDX_MASK   (1 << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

// LED1 placa
#define LED1_PIO            PIOA         
#define LED1_PIO_ID         ID_PIOA  
#define LED1_PIO_IDX        0                   
#define LED1_PIO_IDX_MASK   (1 << LED1_PIO_IDX)   

// LED2 placa
#define LED2_PIO            PIOC                 
#define LED2_PIO_ID         ID_PIOC  
#define LED2_PIO_IDX        30                   
#define LED2_PIO_IDX_MASK   (1 << LED2_PIO_IDX)   

// LED3 placa
#define LED3_PIO            PIOB                 
#define LED3_PIO_ID         ID_PIOB 
#define LED3_PIO_IDX        2                 
#define LED3_PIO_IDX_MASK   (1 << LED3_PIO_IDX)  


// Botão
#define BUT_PIO           PIOA
#define BUT_PIO_ID        ID_PIOA
#define BUT_PIO_IDX       11
#define BUT_PIO_IDX_MASK  (1u << BUT_PIO_IDX) 

// Botão 1 placa
#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        ID_PIOD
#define BUT1_PIO_IDX       28
#define BUT1_PIO_IDX_MASK  (1u << BUT1_PIO_IDX)

// Botão 2 placa
#define BUT2_PIO           PIOC
#define BUT2_PIO_ID        ID_PIOC
#define BUT2_PIO_IDX       31
#define BUT2_PIO_IDX_MASK  (1u << BUT2_PIO_IDX)

// Botão 3 placa
#define BUT3_PIO           PIOA
#define BUT3_PIO_ID        ID_PIOA
#define BUT3_PIO_IDX       19
#define BUT3_PIO_IDX_MASK  (1u << BUT3_PIO_IDX)

/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)



/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void _pio_set(Pio *p_pio, const uint32_t ul_mask);
void _pio_clear(Pio *p_pio, const uint32_t ul_mask);
void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable);
void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute);
void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_multidrive_enable, const uint32_t ul_pull_up_enable);
int _pio_get(Pio *p_pio, const uint32_t ul_type, const uint32_t ul_mask);
void _delay_ms(int ms);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void _pio_set(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_CODR = ul_mask;

}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable)
{
	if (ul_pull_up_enable) {
		
		p_pio->PIO_PUER = ul_mask;
		
	} else {
		
		p_pio->PIO_PUDR = ul_mask;
		
	}

}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute)
{
	_pio_pull_up(p_pio, ul_mask, ul_attribute & _PIO_PULLUP);
	
	if (ul_attribute & (_PIO_DEGLITCH | _PIO_DEBOUNCE)) {
		
		p_pio->PIO_IFER = ul_mask;
		
		} else {
			
		p_pio->PIO_IFDR = ul_mask;
	}
	
	if (ul_attribute & _PIO_DEGLITCH) {
		
		p_pio->PIO_IFSCDR = ul_mask;
	}
	
	if (ul_attribute & _PIO_DEBOUNCE) {
		
		p_pio->PIO_IFSCER = ul_mask;
	}
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_multidrive_enable, const uint32_t ul_pull_up_enable) 
{
	p_pio->PIO_PER = ul_mask;
	
	p_pio->PIO_OER = ul_mask;
	
	if (ul_default_level) {
		
		_pio_set(p_pio, ul_mask);
		
		} else {
			
		_pio_clear(p_pio, ul_mask);
	}
	
	if (ul_multidrive_enable) {
		
		p_pio->PIO_MDER = ul_mask;
		
		} else {
			
		p_pio->PIO_MDDR = ul_mask;
	}
	
	if (ul_pull_up_enable) {
		_pio_pull_up(p_pio, ul_mask, 1);
		
		} else {
			
		_pio_pull_up(p_pio, ul_mask, 0);
	}
}

int _pio_get(Pio *p_pio, const uint32_t ul_type, const uint32_t ul_mask) 
{
	if (ul_type == PIO_OUTPUT_0 || ul_type == PIO_OUTPUT_1) {
		
		return !((p_pio->PIO_ODSR & ul_mask) == 0);
		
		} else {
			
		return !((p_pio->PIO_PDSR & ul_mask) == 0);
	}
}

void _delay_ms(int ms) 
{
	for (int i = 0; i < ms*150000; i++) {
		asm("nop");
	}
}

// Função de inicialização do uC
void init(void){
	// Initialize the board clock
	sysclk_init();

	// Disativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;

	// Ativa o PIO na qual o LED foi conectado para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(LED1_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(LED3_PIO_ID);
	
	//Inicializa PC8 como saída
	_pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 1, 0, 0);
	_pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 1, 0, 0);
	_pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 1, 0, 0);
	_pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 1, 0, 0);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	_pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	_pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	_pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	_pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);

}





/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {
	  // int led1 = pio_get(BUT1_PIO, PIO_INPUT,BUT_PIO_IDX_MASK);	  
	  if(!_pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK)){
		  
		for (int i=0; i<5; i++) {
			_pio_set(PIOC, LED_PIO_IDX_MASK);      // Coloca 1 no pino LED
			_delay_ms(100);                        // Delay por software 
			_pio_clear(PIOC, LED_PIO_IDX_MASK);    // Coloca 0 no pino do LED
			_delay_ms(100);}                        // Delay por software 
	} else  {
			_pio_set(LED_PIO, LED_PIO_IDX_MASK); 
			}
	  
	  
	  if(!_pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)){
		  
		for (int i=0; i<5; i++) { 
		_pio_set(PIOA, LED1_PIO_IDX_MASK);      // Coloca 1 no pino LED
		_delay_ms(100);                        // Delay por software
		_pio_clear(PIOA, LED1_PIO_IDX_MASK);    // Coloca 0 no pino do LED
		_delay_ms(100);}                       // Delay por software
	} else {
	  _pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
  }
	  
	  
	  
	  if(!_pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK)){
		  
		  for (int i=0; i<5; i++) {
		  _pio_set(PIOC, LED2_PIO_IDX_MASK);      // Coloca 1 no pino LED
		  _delay_ms(100);                        // Delay por software
		  _pio_clear(PIOC, LED2_PIO_IDX_MASK);    // Coloca 0 no pino do LED
		  _delay_ms(100);}                      // Delay por software
	  } else  {
	  _pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
  }
	  
	  
	  
	  if(!_pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)){
		  
		  for (int i=0; i<5; i++) {
		  _pio_set(PIOB, LED3_PIO_IDX_MASK);      // Coloca 1 no pino LED
		  _delay_ms(100);                        // Delay por software
		  _pio_clear(PIOB, LED3_PIO_IDX_MASK);    // Coloca 0 no pino do LED
		  _delay_ms(100);}                    // Delay por software
	  } else  {
	  _pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
  }
  }
  return 0;
  }

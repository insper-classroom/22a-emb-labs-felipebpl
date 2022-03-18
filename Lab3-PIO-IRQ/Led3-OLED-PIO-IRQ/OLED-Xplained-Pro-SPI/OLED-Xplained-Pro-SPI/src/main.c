#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO           PIOC
#define LED_PIO_ID        ID_PIOC
#define LED_PIO_IDX       8
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)

#define BUT1_PIO			PIOD
#define BUT1_PIO_ID			ID_PIOD
#define BUT1_PIO_IDX		28
#define BUT1_PIO_IDX_MASK (1 << BUT1_PIO_IDX)

#define BUT2_PIO			PIOC
#define BUT2_PIO_ID			ID_PIOC
#define BUT2_PIO_IDX		31
#define BUT2_PIO_IDX_MASK	(1 << BUT2_PIO_IDX)

#define BUT3_PIO			PIOA
#define BUT3_PIO_ID			ID_PIOA
#define BUT3_PIO_IDX		19
#define BUT3_PIO_IDX_MASK	(1 << BUT3_PIO_IDX)

volatile int freq = 1;
volatile int active = 1;
volatile int curr = 0;
int max = 30;
volatile int but1_flag;
volatile int but3_flag;

void pisca_led(void);
void b1_callback(void);
void b1_pressed(void);
void b1_released(void);
void b2_callback(void);
void b3_callback(void);
void b3_pressed(void);
void b3_released(void);


void pisca_led()
{
	pio_clear(LED_PIO, LED_PIO_IDX_MASK);
	delay_ms((1.0 / freq) * 1000);
	pio_set(LED_PIO, LED_PIO_IDX_MASK);
	delay_ms((1.0 / freq) * 1000);
	curr += 1;
	double a = ((double)curr / max)*120;
	gfx_mono_generic_draw_filled_rect(1, 0, a, 5, 1);
}

void b1_callback() {
	if (!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)) {
		b1_pressed();
		} else {
		b1_released();
	}
}

void b1_pressed()
{
	while (!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)) {
		but1_flag += 1;
		delay_ms(1);
	}
}

void b1_released() {
	if (but1_flag > 500) {
		if (freq > 1) { 
			freq -= 1;
			 }
		} else {
		freq += 1;
	}
	
	but1_flag = 0;
	char str[128];
	sprintf(str, "%d", freq);
	gfx_mono_draw_string("   ", 50,16, &sysfont);
	gfx_mono_draw_string(str, 50,16, &sysfont);
}

void b2_callback()
{
	if (active) {
		active = 0;
		} else {
		active = 1;
	}
}

void b3_callback() {
	if (!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)) {
		b3_pressed();
		} else {
		b3_released();
	}
}

void b3_pressed()
{
	while (!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK)) {
		but3_flag += 1;
		delay_ms(1);
	}
}

void b3_released() {
	if (but3_flag < 500) {
		if (freq > 1) { freq -= 1; }
	}
	
	but3_flag= 0;
	
	char str[128];
	sprintf(str, "%d", freq);
	gfx_mono_draw_string("   ", 50,16, &sysfont);
	gfx_mono_draw_string(str, 50,16, &sysfont);
}

int init (void)
{
	board_init();
	sysclk_init();
	delay_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);	
	
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);

	pmc_enable_periph_clk(BUT2_PIO_ID);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_PIO_IDX_MASK, 60);
	
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);
	
	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_PIO_IDX_MASK,
	PIO_IT_EDGE,
	b1_callback);

	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);

	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4);
	
	
	pio_handler_set(BUT2_PIO,
	BUT2_PIO_ID,
	BUT2_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	b2_callback);

	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT2_PIO);

	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4);
	
	
	pio_handler_set(BUT3_PIO,
	BUT3_PIO_ID,
	BUT3_PIO_IDX_MASK,
	PIO_IT_EDGE,
	b3_callback);

	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT3_PIO);

	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4);
	
	gfx_mono_ssd1306_init();
	
}

//int configure_pushbutton()

int main (void)
{
	init();
	
	char str[128];
	sprintf(str, "%d", freq);
	gfx_mono_draw_string(str, 50,16, &sysfont);
	gfx_mono_draw_string("Hz", 100,16, &sysfont);
	
	while(1)
	{
		if (active && curr <= max) {
			pisca_led();
		}
	}
}
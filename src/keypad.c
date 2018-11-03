#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <string.h>


void display1(const char *);
void display2(const char *);
void update_freq(int f);
void update_rgb(int r, int g, int b);
void nano_wait(int);

//The keypad row currently being examined.
static int row = 0;

//Turn on one row at a time.

void scan_keypad(void) {
	for(;;)
		for(row=1; row<5; row++) {
			GPIOA->BSRR = 1 << (row+3);
			nano_wait(10000000);
			GPIOA->BRR = 0xf0;
 			nano_wait( 1000000);
		}
}

static void press(char c) {
	static int offset = 0;
	static int r,g,b;
	static int freq = 0;
	static char ledline[17];
	static char freqline[17];
	static int entry = ' ';
	if (c == '*') {
		if (entry != ' ') return;
		entry = '*';
		strcpy(ledline, "LED: _");
		display1(ledline);
		return;
	}
	if (c == '#') {
		if(entry != ' ') return;
		entry = '#';
		strcpy(freqline, "FREQ: _");
		display2(freqline);
		return;
	}

	if (entry == '*') {
		ledline[5 + offset] = c;
		if (offset < 5)
			ledline[5 + offset + 1] = '_';
		display1(ledline);
		switch(offset) {
		case 0: r = 10 * (c-'0');  offset++; break;
		case 1: r += (c - '0');    offset++; break;
		case 2: g = 10 * (c- '0'); offset++; break;
		case 3: g += (c - '0');    offset++; break;
		case 4: b = 10 * (c-'0');  offset++; break;
		case 5: b += (c - '0');    offset=0; update_rgb(r,g,b); entry=' '; break;
		}
	}
	if (entry == '#') {
		freqline[6+offset] = c;
		if (offset < 5)
			freqline[6 + offset + 1] = '_';
		display2(freqline);
		freq = freq * 10 + (c - '0');
		offset += 1;
		if (offset == 6) {
			if (freq < 8) {
				freq = 8;
				display2("FREQ: 000008");
			}
			if (freq > 480000) {
				freq = 480000;
				display2("FREQ: 480000");
			}
			offset = 0;
			entry = ' ';
			update_freq(freq);
			freq = 0;
		}
	}

}

//void TIM2_IRQHandler() {
//	if ((TIM2->SR & TIM_SR_UIF) != 0) {
//		TIM2->SR &= ~TIM_SR_UIF;
//		return;
//	}
//
//	if (TIM2->SR & TIM_SR_CC2IF) {
//		switch(row) {
//		case 1: press('1'); break;
//		case 2: press('4'); break;
//		case 3: press('7'); break;
//		case 4: press ('*'); break;
//		}
//	}
//
//	if (TIM2->SR & TIM_SR_CC3IF) {
//		switch(row) {
//		case 1: press('2'); break;
//		case 2: press('5'); break;
//		case 3: press('8'); break;
//		case 4: press('0'); break;
//		}
//	}
//
//	if (TIM2->SR & TIM_SR_CC4IF) {
//		switch(row) {
//		case 1: press('3'); break;
//		case 2: press('6'); break;
//		case 3: press('9'); break;
//		case 4: press('#'); break;
//		}
//	}
//
//	nano_wait(10 * 1000 * 1000);
//
//	while((GPIOA->IDR & 0xf) != 0)
//		;
//
//	nano_wait(10 * 1000 * 1000);
//
//	//////////////////////////////////
//	int __attribute((unused)) useless;
//	useless = TIM2->CCR2;
//	useless = TIM2->CCR3;
//	useless = TIM2->CCR4;
//
//	TIM2->SR &= ~TIM_SR_UIF;
//
//	static int seconds = 0;
//
//	static int minutes = 0;
//
//	static int hours = 0;
//
//		seconds += 1;
//
//		if (seconds > 59)
//		{
//			seconds = 0;
//			minutes += 1;
//
//		}
//
//		if (minutes > 59) {
//
//			minutes = 0;
//
//			hours += 1;
//
//		}
//
//		char line[20];
//
//		sprintf(line, "Waiting %02d:%02d:%02d", hours, minutes, seconds);
//		display11(line);
//	return;
//
//}


void init_keypad(void) {


	 	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	 	GPIOA->MODER |= 0X5500;
	 	GPIOA->BRR |= 0xf0;
	 	GPIOA->PUPDR &= ~(3<<2);
	 	GPIOA->PUPDR |= 2<<2;
	 	GPIOA->PUPDR |= 2<<4;
	 	GPIOA->PUPDR |= 2<<6;
	 	GPIOA->MODER |= 0xA8;
	 	GPIOA->AFR[0] |= 0x2220;
	 	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	 	//channel 2
	 	TIM2->PSC = 0;
	 	TIM2->ARR = 0xffffffff;

	 	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
	 	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
	 	TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
	 	TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;

	 	TIM2->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP);
	 	TIM2->CCER |= TIM_CCER_CC2E;
	 	TIM2->DIER |= TIM_DIER_CC2IE;
	 	TIM2->CR1 |= TIM_CR1_CEN;
	 	NVIC->ISER[0] = 1<<TIM2_IRQn;

	 	TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
	 	TIM2->CCMR1 |= TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2 |
	 	TIM_CCMR1_IC1F_1 |TIM_CCMR1_IC1F_0;

	 	TIM2->CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;
	 	//channel 3

	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
	 	 	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;

	 	 	TIM2->CCER &= ~(TIM_CCER_CC3P|TIM_CCER_CC3NP);
	 	 	TIM2->CCER |= TIM_CCER_CC3E;
	 	 	TIM2->DIER |= TIM_DIER_CC3IE;
	 	 	//TIM2->CR1 |= TIM_CR1_CEN;
	 	 	//NVIC->ISER[0] = 1<<TIM2_IRQn;

	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
	 	 	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 |
	 	 	TIM_CCMR2_IC3F_1 |TIM_CCMR2_IC3F_0;

	 	 	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;


	 	 	//channel 4

	 	 	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
	 	 	 	 	TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
	 	 	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
	 	 	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;

	 	 	 	 	TIM2->CCER &= ~(TIM_CCER_CC4P|TIM_CCER_CC4NP);//unsure
	 	 	 	 	TIM2->CCER |= TIM_CCER_CC4E;
	 	 	 	 	TIM2->DIER |= TIM_DIER_CC4IE;
	 	 	 	 	//TIM2->CR1 |= TIM_CR1_CEN;
	 	 	 	 	//NVIC->ISER[0] = 1<<TIM2_IRQn;

	 	 	 	 	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
	 	 	 	 	TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 |
	 	 	 	 	TIM_CCMR2_IC4F_1 |TIM_CCMR2_IC4F_0;

	 	 	 	 	TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 | TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;
}

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

void init_lcd(void);
void display1(const char *);
void display2(const char *);

void init_pwm(void);
void update_freq(int f);
void update_rgb(int r, int g, int b);

void init_keypad(void);
void scan_keypad(void);

void display11(const char *);
void init_lcd2(void);


void TIM2_IRQHandler(void) {
	TIM2->SR &= ~TIM_SR_UIF;

static int seconds = 0;

static int minutes = 0;

static int hours = 0;

	seconds += 1;

	if (seconds > 59)
	{
		seconds = 0;
		minutes += 1;

	}

	if (minutes > 59) {

		minutes = 0;

		hours += 1;

	}

	char line[20];

	sprintf(line, "Waiting %02d:%02d:%02d", hours, minutes, seconds);

	display11(line);
}

void init_tim2(void) {

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
		TIM2->PSC = 48 - 1;
		TIM2->ARR = 1000000 - 1;
		TIM2->CR1 &= ~TIM_CR1_DIR;
		TIM2->DIER |= TIM_DIER_UIE;
		TIM2->CR1 |= TIM_CR1_CEN;
		NVIC->ISER[0] = 1<<TIM2_IRQn;
}

int main(void)
{
//    init_keypad();
    init_lcd();
//    init_pwm();
//    update_rgb(1,1,1);
//    update_freq(8);
    display1("SOLVE:");
    display2("3*(3+3) = ");
//    scan_keypad();

    init_lcd2();
    init_tim2();
    for(;;) asm("wfi");
}


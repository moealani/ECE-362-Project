#include "stm32f0xx.h"
#include "stm32f0_discovery.h"


#define SCL 1
#define SDI 2
#define SS 4
#define SPI_DELAY 400


void nano_wait(int t);

static void sendbit(int b)
{
	GPIOC->BRR = SCL;
	GPIOC->BSRR = b ? SDI : (SDI << 16);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SCL;
	nano_wait(SPI_DELAY);
}

static void sendbyte(char b)
{
	int x;
	for(x=8; x>0; x--)
	{
		sendbit(b & 0x80);
		b <<= 1;
	}
}


static void cmd(char b)
{
	GPIOC-> BRR = SS;
	nano_wait(SPI_DELAY);
	sendbit(0);
	sendbit(0);
	sendbyte(b);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SS;
	nano_wait(SPI_DELAY);
}

static void data(char b)
{
	GPIOC->BRR = SS;
	nano_wait(SPI_DELAY);
	sendbit(1);
	sendbit(0);
	sendbyte(b);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SS;
	nano_wait(SPI_DELAY);
}




void init_lcd(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->ODR |= 7;
	GPIOC->MODER &= ~0x3f;
	GPIOC->MODER |= 0x15;

	nano_wait(100000000);
	cmd(0x38);
	cmd(0x0c);
	cmd(0x01);
	nano_wait(6200000);
	cmd(0x02);
	cmd(0x06);
}

void display1(const char *s)
{
	cmd(0x02);
	int len;
	for(len=0; len<16; len += 1)
	{
		if(s[len] == '\0')
			break;
		data(s[len]);
	}
}

void display2(const char *s)
{
	cmd(0xc0);
	int len;
	for(len=0; len<16; len += 1)
	{
		if(s[len] == '\0')
			break;
		data(s[len]);
	}
}

void init_pwm(void) {
	GPIOA->MODER |= 0x2a0000;
	GPIOA->AFR[1] &= ~(0xf<<(4*0));
	GPIOA->AFR[1] |= 0x2<<(4*0);
	GPIOA->AFR[1] &= ~(0xf<<(4*1));
	GPIOA->AFR[1] |= 0x2<<(4*1);
	GPIOA->AFR[1] &= ~(0xf<<(4*2));
	GPIOA->AFR[1] |= 0x2<<(4*2);

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = 1000;
	TIM1->ARR = 100 - 1;
	TIM1->CCR1 = 99;
	TIM1->CCR2 = 99;
	TIM1->CCR3 = 99;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
	TIM1->CCER |= TIM_CCER_CC1E;
	TIM1->CCER |= TIM_CCER_CC2E;
	TIM1->CCER |= TIM_CCER_CC3E;
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CR1 |= TIM_CR1_CEN;
//	TIM1->EGR |= TIM_EGR_UG;
}


void update_freq(int freq) {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = (48000000 / (freq * 100)) - 1.0;
}


void update_rgb(int r, int g, int b) {
	TIM1->CCR1 = r;
	TIM1->CCR2 = g;
	TIM1->CCR3 = b;
}

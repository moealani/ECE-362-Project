#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdlib.h>
#include <stdio.h>

void init_lcd(void);
void display3(const char *);
void display4(const char *);
void press(char);

void init_pwm(void);
void update_freq(int f);
void update_rgb(int r, int g, int b);

void init_keypad(void);
void scan_keypad(void);

void display1(const char *);
void display2(const char *);
void init_lcd2(void);

void math_time();
void alarmON(void);

char timeArr[16]; // used to store time displayed
char input[3]; // used to store user input for answer
char ans[3]; // used to store answer of generated math problem
char input_time[16];
int setting = 2;
int timeofday = 9;
int cond = 0;
int alarmSound = 0; // 1 if alarm sound is active (use to trigger sound)
int alarmSet = 0;
char ansArr[9]; //
static int row = 0;
int entry = ' ';
int seed;
int onlyOnce = 0;

void alarmON(void)
{
	if(onlyOnce != 1)
	{
		math_time(ans, ansArr);
		onlyOnce++;
	}
	if(timeArr[3] == input_time[0] && timeArr[4] == input_time[1] && timeArr[6] == input_time[2] && timeArr[7] == input_time[3] && timeofday == setting)
	{
		alarmSound = 1;
		char opt[20] = {'Q',':', '(', ansArr[0], ' ', ansArr[3] , ' ', ansArr[1], ')', ' ', ansArr[4], ' ', ansArr[2], ' ', '=', ' '};
		display3(opt);
	}
	else
	{

	}
}

void scan_keypad(void) {

	int x = 0;
	int y = 1;
	int z = 0;

    for(;;)
    {
    	for(row=1; row<5; row++)
    	{
    		GPIOC->BSRR = 1 << (row+3);
            nano_wait(10000000);
            GPIOC->BRR = 0xf0;
            nano_wait(1000000);
            //DAC BEEP
            if (cond != 1)
            {
            	if (alarmSound == 1)
            	{
            		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
            		GPIOA->MODER |= 2<<(2*4);
            		RCC->APB1ENR |= RCC_APB1ENR_DACEN;
            		DAC->CR &= ~DAC_CR_EN1;
            		DAC->CR &= ~DAC_CR_BOFF1;
            		DAC->CR |= DAC_CR_TEN1;
            		DAC->CR |= DAC_CR_TSEL1;
            		DAC->CR |= DAC_CR_EN1;

            		while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
            		DAC->DHR12R1 = x;
            		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
            		if(z == 0x0)
            		{
            			y = 1;
            		}
            		else if(z == 0x7fff)
            		{
            			y = 0;
            		}
            		z = (z+1) & 0xffff;
            		if(y == 1)
            		{
            			x = (x+40) & 0xfff;
            		}
            		else
            		{
            			x = 0;
            		}
            	}
            }
            else if (cond == 1)
            {
            	//display2("GANG");
            	DAC->CR |= DAC_CR_TEN1;
            	DAC->CR |= DAC_CR_TSEL1;
            	DAC->CR &= ~DAC_CR_BOFF1;
            	DAC->CR &= ~DAC_CR_EN1;
            	GPIOA->MODER &= ~2<<(2*4);
            	RCC->APB1ENR &= ~RCC_APB1ENR_DACEN;
            	RCC->AHBENR &= ~RCC_AHBENR_GPIOAEN;
            }
    	}
    }
}

void TIM2_IRQHandler() {
    if ((TIM2->SR & TIM_SR_UIF) != 0) {
        TIM2->SR &= ~TIM_SR_UIF;
        return;
    }

    if (TIM2->SR & TIM_SR_CC2IF) {
        switch(row) {
        case 1: press('1'); break;
        case 2: press('4'); break;
        case 3: press('7'); break;
        case 4: press('*'); break;
        }
    }

    if (TIM2->SR & TIM_SR_CC3IF) {
        switch(row) {
        case 1: press('2'); break;
        case 2: press('5'); break;
        case 3: press('8'); break;
        case 4: press('0'); break;
        }
    }

    if (TIM2->SR & TIM_SR_CC4IF) {
        switch(row) {
        case 1: press('3'); break;
        case 2: press('6'); break;
        case 3: press('9'); break;
        case 4: press('#'); break;
        }
    }

    nano_wait(10 * 1000 * 1000);
    while((GPIOA->IDR & 0xf) != 0);
    nano_wait(10 * 1000 * 1000);

    int __attribute((unused)) useless;
    useless = TIM2->CCR2;
    useless = TIM2->CCR3;
    useless = TIM2->CCR4;

    return;
}

void init_keypad(void) {

        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        //ENABLE PC3
        RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
        GPIOC->MODER |= 0X5500; //set in output mode
        GPIOC->BRR |= 0xf0;
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
        NVIC_SetPriority(TIM2_IRQn,2);

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

        TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
        TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 |
        TIM_CCMR2_IC3F_1 |TIM_CCMR2_IC3F_0;

        TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;


        //channel 4
        TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
        TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
        TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;

        TIM2->CCER &= ~(TIM_CCER_CC4P|TIM_CCER_CC4NP); //unsure
        TIM2->CCER |= TIM_CCER_CC4E;
        TIM2->DIER |= TIM_DIER_CC4IE;

        TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
        TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 |
        TIM_CCMR2_IC4F_1 |TIM_CCMR2_IC4F_0;

        TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 | TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;
}

//the function to run the clock, from (Sriyash Mirthipati)
ErrorStatus initRtc(void) {
    // Enable PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Enable the Backup Domain Access */
    PWR_BackupAccessCmd(ENABLE);

    /* LSI Enable */
    RCC_LSICmd(ENABLE);

    /* Wait until the LSI crystal is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){
    }

    /* Set RTC clock source to LSI */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);


    /* Enable RTC clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Waits until the RTC Time and Date registers are synchronized with RTC APB
     * clock.*/
    if(RTC_WaitForSynchro() == ERROR) {
        return ERROR;
    }

    // setup the RTC prescalers and 12-hour mode
    RTC_InitTypeDef init;
    RTC_StructInit(&init);
    init.RTC_HourFormat = RTC_HourFormat_12;
    if(RTC_Init(&init) == ERROR) {
        return ERROR;
    }

    RTC_TimeTypeDef startTime = {11, 22, 50, RTC_H12_PM};
    if(RTC_SetTime(RTC_Format_BIN, &startTime) == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}

void rtcGetTime(char* timeStr) {
    RTC_TimeTypeDef time;
    RTC_GetTime(RTC_Format_BIN, &time);
    sprintf(timeStr, "   %02d:%02d:%02d %s",
    		time.RTC_Hours, time.RTC_Minutes,
			time.RTC_Seconds,
			(time.RTC_H12 == RTC_H12_AM ? "AM" : "PM"));

    if (timeStr[9] == 'A')
    {
        timeofday = 1;
    }
    else
    {
        timeofday = 0;
    }

}

void TIM3_IRQHandler(void)
{
    TIM3->SR &= ~TIM_SR_UIF;

    seed += 1;
    srand(seed);
    if(alarmSet == 1)
    {
        alarmON();
    }

    rtcGetTime(timeArr);
    display1(timeArr);
}

void init_tim3(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 6 - 1;
    TIM3->ARR = 6000000 - 1;
    TIM3->CR1 &= ~TIM_CR1_DIR;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM3_IRQn); /* (1) */
    NVIC_SetPriority(TIM3_IRQn,0); /* (2) */
}

//This function is used for conditions when using the keypad for inputs
void press(char c) {
	static int offset = 0;
    static char freqline[17];
    static char settimeline[17];

    if ((c == '*' || c == '#') && alarmSound == 1 && entry != '(') {
        entry = '(';
        strcpy(freqline, "A: _         ");
        display4(freqline);
        return;
    }
    else if (c == '*' && entry != '#' && entry != '^' && entry != '%' && entry != '(' && entry != '!' && alarmSound != 1)
    {
        entry = '$';
    }

    if (entry == '(')
    {
    	freqline[3+offset] = c;
    	if (offset < 2)
    		freqline[3 + offset + 1] = '_';
    	if (c != '*')
    	{
    		input[offset] = c;
    		display4(freqline);
    	}
    	else
    	{
    		int j = 0;
    		while (j < 3)
    		{
    			if (input[j] == ans[j])
    			{
    				cond = 1;
    			}
    			else
    			{
    				cond = 0;
    				break;
    			}
    			j++;
    		}

    		if (cond == 1)
    		{
    			display4("    CORRECT!");
    			return;
    		}
    		else
    		{
    			display4("                           ");
    			display4("   TRY AGAIN!");
    			offset = 0;
    			reset(input);
    		}
    		return;
    	}
    	offset += 1;
    }

    if (entry == '$')
    {
    	display3("                ");
        display3("   SET ALARM?   ");
        display4(" YES: #    NO: *");
        if (c == '#')
        {
           display3("ENTER TIME:           ");
           display4("                      ");
           entry = '^';
           offset = 0;
        }
    }
    if (entry == '^')
    {
        settimeline[offset] = c;
        if (offset < 8)
        	settimeline[offset+1] = '_';

        if (c == '#')
        {
        	return;
        }
        else if (c != '*')
        {
            input_time[offset] = c;
            display4(input_time);
        }
        else if (c == '*')
        {
        	display4("                 ");
            entry = '%';
            c = ')';
        }
        offset += 1;
    }
    if (entry == '%')
    {
    	if (c == '#')
        {
    		setting = 1;
            offset = 0;
            alarmSet = 1;
            entry = '!';
        }
        else if (c == '*')
        {
            setting = 0;
            offset = 0;
            alarmSet = 1;
            entry = '!';
        }
        else
        {
            display3("AM: #      PM: *");
        }
    }
}

void reset(char* ans)
{
    int i = 0;
    while (i < 3)
    {
        ans[i] = '\0';
        i++;
    }
}

int main(void) {

    init_lcd();
    init_lcd2();
    init_pwm();
    init_keypad();
    display3("TO SET ALARM \"*\" ");
    initRtc();
    init_tim3();
    scan_keypad();

    for(;;) asm("wfi");
}

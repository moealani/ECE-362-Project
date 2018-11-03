#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

static short dispmem[] = {
        0x002,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x0c0,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
};

//===========================================================================
// Send a command to the LCD.
void cmd(char b) {
    while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
    SPI2->DR = b;
}

//===========================================================================
// Send a character to the LCD.
void data(char b) {
    while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
    SPI2->DR = 0x200 | b;
}

//===========================================================================
// Initialize the LCD.
void init_lcd_spi(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	GPIOB->MODER |= (2 << (2 * 15));
	GPIOB->MODER |= (2 << (2 * 13));
	GPIOB->MODER |= (2 << (2 * 12));

	GPIOB->AFR[1] |= (4 << (4 * 7));
	GPIOB->AFR[1] |= (4 << (4 * 5));
	GPIOB->AFR[1] |= (4 << (4 * 4));

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_BR; //_0, _1, _2
	SPI2->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
	SPI2->CR2 = SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_DS_3 | SPI_CR2_DS_0;
	SPI2->CR2 |= SPI_CR2_TXDMAEN;
	SPI2->CR1 |= SPI_CR1_SPE;

	nano_wait(100000000); // Give it 100ms to initialize
	cmd(0x38); // 0011 NF00 N=1, F=0: two lines
	cmd(0x0c); // 0000 1DCB: display on, no cursor, no blink
	cmd(0x01); // clear entire display
	nano_wait(6200000); // clear takes 6.2ms to complete
	cmd(0x02); // put the cursor in the home position
	cmd(0x06); // 0000 01IS: set display to increment

}

//===========================================================================
// Initialize the LCD to use circular DMA to write to the SPI channel.
void init_lcd2(void) {
		init_lcd_spi();
		RCC->AHBENR |= RCC_AHBENR_DMA1EN;
		DMA1_Channel5->CCR &= ~DMA_CCR_EN;
		DMA1_Channel5->CMAR = dispmem;
		DMA1_Channel5->CPAR =  &(SPI2->DR);
		DMA1_Channel5->CNDTR = 34;
		SPI2->CR2 |= SPI_CR2_TXDMAEN;
		DMA1_Channel5->CCR |= DMA_CCR_DIR;

		DMA1_Channel5->CCR &= ~DMA_CCR_HTIE;
		DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;
		DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0;
		DMA1_Channel5->CCR |= DMA_CCR_MINC;
		DMA1_Channel5->CCR |= DMA_CCR_CIRC;
		DMA1_Channel5->CCR &= ~DMA_CCR_MEM2MEM;
		DMA1_Channel5->CCR &= ~DMA_CCR_PL;
		DMA1_Channel5->CCR |= DMA_CCR_EN;
}

//===========================================================================
// Display a string on line 1 by writing to SPI directly.
void display1_spi(const char *s) {
    cmd(0x02); // put the cursor on the beginning of the first line.
    int x;
    for(x=0; x<16; x+=1)
        if (s[x])
            data(s[x]);
        else
            break;
    for(   ; x<16; x+=1)
        data(' ');
}

//===========================================================================
// Display a string on line 2 by writing to SPI directly.
void display2_spi(const char *s) {
    cmd(0xc0); // put the cursor on the beginning of the second line.
    int x;
    for(x=0; x<16; x+=1)
        if (s[x] != '\0')
            data(s[x]);
        else
            break;
    for(   ; x<16; x+=1)
        data(' ');
}

//===========================================================================
// Display a string on line 1 by using DMA to write to SPI.
void display1_dma(const char *s) {
    cmd(0x02); // put the cursor on the beginning of the first line.
    int x;
    for(x=0; x<16; x+=1)
        if (s[x])
           dispmem[x + 1] = s[x] | 0x200;
        else
            break;
    for(   ; x<16; x+=1)
       	dispmem[x+1] = 0x220;

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;
    DMA1_Channel5->CMAR = dispmem;
    DMA1_Channel5->CPAR =  &(SPI2->DR);
    DMA1_Channel5->CNDTR = 17;
    DMA1_Channel5->CCR |= DMA_CCR_DIR;
    DMA1_Channel5->CCR &= ~DMA_CCR_HTIE;
    DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel5->CCR |= DMA_CCR_MINC;
    DMA1_Channel5->CCR &= ~DMA_CCR_MEM2MEM;
    DMA1_Channel5->CCR &= ~DMA_CCR_PL;
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

//===========================================================================
// Display a string on line 2 by using DMA to write to SPI.
void display2_dma(const char *s) {

}

//===========================================================================
// Display a string on line 1 by writing to the DMA sorrce.
void display11(const char *s) {
    int x;
    for(x=0; x<16; x+=1)
        if (s[x])
           dispmem[x + 1] = s[x] | 0x200;
        else
            break;
    for(   ; x<16; x+=1)
       	dispmem[x+1] = 0x220;

}

//===========================================================================
// Display a string on line 2 by writing to the DMA source.
void display12(const char *s) {
    int x;
    for(x=0; x<16; x+=1)
        if (s[x])
           dispmem[x + 18] = s[x] | 0x200;
        else
            break;
    for(   ; x<16; x+=1)
       	dispmem[x+18] = 0x220;

}

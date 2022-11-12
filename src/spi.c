//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### SPI.C #################################
//---------------------------------------------
#include "spi.h"
#include "stm32g0xx.h"
#include "hard.h"


// Module Private Types Constants and Macros -----------------------------------
#define RCC_SPI1_CLK    (RCC->APBENR2 & 0x00001000)
#define RCC_SPI1_CLK_ON    (RCC->APBENR2 |= 0x00001000)
#define RCC_SPI1_CLK_OFF    (RCC->APBENR2 &= ~0x00001000)

#define RCC_SPI2_CLK    (RCC->APBENR1 & 0x00004000)
#define RCC_SPI2_CLK_ON    (RCC->APBENR1 |= 0x00004000)
#define RCC_SPI2_CLK_OFF    (RCC->APBENR1 &= ~0x00004000)


// Externals -------------------------------------------------------------------
extern volatile unsigned char TxBuffer_SPI [];
extern volatile unsigned char RxBuffer_SPI [];
extern volatile unsigned char *pspi_tx;
extern volatile unsigned char *pspi_rx;
extern volatile unsigned char spi_bytes_left;


// Globals ---------------------------------------------------------------------



// Module Functions ------------------------------------------------------------
void SPI_Config(void)
{
    if (!RCC_SPI1_CLK)
        RCC_SPI1_CLK_ON;

    //Configuracion SPI
    SPI1->CR1 = 0;

#if defined SPI_MASTER
    // //SPI speed; clk / 256; master
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 |
        SPI_CR1_MSTR;
    //SPI speed; clk / 16; master
    // SPI1->CR1 |=  SPI_CR1_BR_1 | SPI_CR1_BR_0;
    //SPI speed; clk / 8; master
    // SPI1->CR1 |=  SPI_CR1_BR_1;
    //SPI speed; clk / 4; master
    // SPI1->CR1 |=  SPI_CR1_BR_0;
#elif defined SPI_SLAVE
    //SPI speed; clk / 256; master
    //TODO: chequear esta configuracion, la hice de memoria
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 |SPI_CR1_SSM;
#else
#error "Select Peripheral use on spi.h"
#endif

#ifdef SPI_DATA_VALID_ON_RISING_CLK
#ifdef SPI_CLK_IDLE_RESET
    //CPOL Low; CPHA first clock
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
#endif
#ifdef SPI_CLK_IDLE_SET
    //CPOL High; CPHA first clock
    SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_SSM | SPI_CR1_SSI;
#endif    
#endif
    
#ifdef SPI_DATA_VALID_ON_FALLING_CLK
#ifdef SPI_CLK_IDLE_RESET    
    //CPOL Low; CPHA second clock
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPHA;
#endif
#ifdef SPI_CLK_IDLE_SET    
    //CPOL High; CPHA second clock    
    SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPHA;
#endif
#endif
    
    //thresh 8 bits; data 8 bits;
    SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

    unsigned int temp;
    temp = GPIOA->AFR[0];
    temp &= 0x000FFFFF;
    temp |= 0x00000000;	//PA7 -> AF0 PA6 -> AF0 PA5 -> AF0
    GPIOA->AFR[0] = temp;

    //habilito periferico
    SPI1->CR1 |= SPI_CR1_SPE;    
}


unsigned char SPI_Send_Receive (unsigned char a)
{
    unsigned char rx;

    //primero limpio buffer rx spi
    while ((SPI1->SR & SPI_SR_RXNE) == 1)
    {
        rx = SPI1->DR & 0x0F;
        rx += 1;    //for compilation warnings!
    }

    //espero que haya lugar en el buffer
    while ((SPI1->SR & SPI_SR_TXE) == 0);

    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

    //espero tener el dato en RX
    for (unsigned char j = 0; j < 150; j++)
    {
    	asm("nop");
    }

    return (SPI1->DR);
}


void SPI_Busy_Wait (void)
{
    //espero que se transfiera el dato
    while ((SPI1->SR & SPI_SR_BSY) != 0);
}


void SPI_Send_Multiple (unsigned char a)
{
    //espero que haya lugar en el buffer
    while ((SPI1->SR & SPI_SR_TXE) == 0);

    //*(__IO uint8_t *) SPI1->DR = a;
    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

}


void SPI_Send_Single (unsigned char a)
{
    //espero que se libere el buffer
    while ((SPI1->SR & SPI_SR_TXE) == 0);

    //tengo espacio
    //SPI1->DR = a;
    //SPI1->DR = a;
    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

    //espero que se transfiera el dato
    while ((SPI1->SR & SPI_SR_BSY) != 0);
}


unsigned char SPI_Receive_Single (void)
{
    unsigned char dummy;

    //espero que se libere el buffer
    while (((SPI1->SR & SPI_SR_TXE) == 0) || ((SPI1->SR & SPI_SR_BSY) != 0));

    //limpio buffer RxFIFO
    while ((SPI1->SR & SPI_SR_RXNE) != 0)
        dummy = SPI1->DR;

    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = 0xff; //evito enviar 16bits problemas de compilador

    //espero que se transfiera el dato
    while ((SPI1->SR & SPI_SR_BSY) != 0);

    dummy = (unsigned char) SPI1->DR;
    return dummy;
}


void SPI_Chip_Select_On (void)
{
    CSN_ON;
}


void SPI_Chip_Select_Off (void)
{
    CSN_OFF;    
}


//--- end of file ---//

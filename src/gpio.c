//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### GPIO.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32g0xx.h"
#include "gpio.h"
#include "hard.h"


// Module Private Types Constants and Macros -----------------------------------
// - Port enables
#define GPIOA_ENABLE
#define GPIOB_ENABLE
//#define GPIOC_ENABLE

// - Ports Clocks
#define GPIOA_CLK    (RCC->IOPENR & 0x00000001)
#define GPIOA_CLK_ON    (RCC->IOPENR |= 0x00000001)
#define GPIOA_CLK_OFF    (RCC->IOPENR &= ~0x00000001)

#define GPIOB_CLK    (RCC->IOPENR & 0x00000002)
#define GPIOB_CLK_ON    (RCC->IOPENR |= 0x00000002)
#define GPIOB_CLK_OFF    (RCC->IOPENR &= ~0x00000002)

#define GPIOC_CLK    (RCC->IOPENR & 0x00000004)
#define GPIOC_CLK_ON    (RCC->IOPENR |= 0x00000004)
#define GPIOC_CLK_OFF    (RCC->IOPENR &= ~0x00000004)

#define GPIOD_CLK    (RCC->IOPENR & 0x00000008)
#define GPIOD_CLK_ON    (RCC->IOPENR |= 0x00000008)
#define GPIOD_CLK_OFF    (RCC->IOPENR &= ~0x00000008)

#define GPIOF_CLK    (RCC->IOPENR & 0x00000020)
#define GPIOF_CLK_ON    (RCC->IOPENR |= 0x00000020)
#define GPIOF_CLK_OFF    (RCC->IOPENR &= ~0x00000020)

// #define SYSCFG_CLK (RCC->APB2ENR & 0x00000001)
// #define SYSCFG_CLK_ON RCC->APB2ENR |= 0x00000001
// #define SYSCFG_CLK_OFF RCC->APB2ENR &= ~0x00000001


//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
    //--- MODER ---//
    //00: Input mode 
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode (reset state)

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved

#ifdef HARDWARE_VER_1_0
    unsigned long temp;

#ifdef GPIOA_ENABLE    
    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;    //2 bits por pin
    temp &= 0x3CC3030F;    // PA2 - PA3 alternate; PA5 - PA7 alternative
    temp |= 0x0128A8A0;    // PA9 - PA10 alternative; PA12 output; PA15 input
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFBFF;
    temp |= 0x00000400;       
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFCC333CF;
    temp |= 0x00000000;    // PA2, PA5, PA7, PA9, PA10, PA12 low speed
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
    temp &= 0x3FC3FF3F;    // PA3, PA9, PA10, PA15 pullups
    temp |= 0x40140040;
    GPIOA->PUPDR = temp;

#endif    // GPIOA_ENABLE
    
#ifdef GPIOB_ENABLE    
    //--- GPIO B ---//
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;    //2 bits por pin
    temp &= 0xFFFCFFCC;    // PB0 output; PB2 analog
    temp |= 0x00000031;    // PB8 input
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;    //1 bit por pin
    temp &= 0xFFFFFFFF;    // PB1 open drain
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;    //2 bits por pin
    temp &= 0xFFFFFFFC;
    temp |= 0x00000000;    // PB0 low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;    //2 bits por pin
    temp &= 0xFFFCFFFF;    //PB8 pullup
    temp |= 0x00010000;
    GPIOB->PUPDR = temp;

#endif    // GPIOB_ENABLE
#endif    //#ifdef HARDWARE_VER_1_0
    

//	//Interrupt en PB8
//	if (!SYSCFG_CLK)
//		SYSCFG_CLK_ON;
//
//	SYSCFG->EXTICR[2] = 0x00000001; //Select Port B & Pin 8 external interrupt
//	EXTI->IMR |= 0x0100; 			//Corresponding mask bit for interrupts
//	EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
//	EXTI->RTSR |= 0x0100; 			//Pin 8 Interrupt line on rising edge
//	EXTI->FTSR |= 0x0100; 			//Pin 8 Interrupt line on falling edge
//
//	NVIC_EnableIRQ(EXTI4_15_IRQn);
//	NVIC_SetPriority(EXTI4_15_IRQn, 6);
}

inline void EXTIOff (void)
{
    // EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
    // EXTI->IMR |= 0x00000100;
}

//--- end of file ---//

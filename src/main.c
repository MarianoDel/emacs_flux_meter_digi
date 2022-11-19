//----------------------------------------------------
// #### PROJECT Digital Flux Meter - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C #######################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32g0xx.h"
#include "hard.h"
#include "gpio.h"
#include "tim.h"

#include "adc.h"
#include "dma.h"
#include "usart.h"

#include "i2c.h"
#include "spi.h"

#include "screen.h"
#include "ssd1306_display.h"

#include "measure.h"

#include "test_functions.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>





// Externals -------------------------------------------------------------------
// - Externals from ADC Converter -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

// - Externals de la Memoria y los modos -------
// parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_ADDRESS_FOR_BKP;	//en flash
// parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
// - Globals from timers -------
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_led = 0;
// volatile unsigned short timer_prender_ringing = 0;
// volatile unsigned short mins_millis = 0;


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);
// void ConfigurationChange (void);



//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    //Start the SysTick Timer
#ifdef CLOCK_FREQ_64_MHZ
    if (SysTick_Config(64000))
        SysTickError();
#endif
#ifdef CLOCK_FREQ_16_MHZ
    if (SysTick_Config(16000))
        SysTickError();
#endif

    //--- Hardware Test Functions ---
    // TF_Hardware_Tests ();
    
    //--- End of Hardware Test Functions ---
    
    
    //--- Begin Production Program ---
    // --- Start Hardware Configs
    Wait_ms(500);    //for supply stability
    LED_ON;
    
    // init I2C
    I2C1_Init();

    // init Usart
    Usart2Config();
    
    // init SPI
    SPI_Config ();

    // Oled init - first screen
    SCREEN_Init();

    timer_standby = 2000;
#ifdef GAUSSTEK_METER
    SCREEN_ShowText2(
        "Gausstek ",
        "  Flux   ",
        "    Meter",
        "         "        
        );
#endif
#ifdef INFINTY_METER
    SCREEN_ShowText2(
        "Infinity ",
        "  Clinics",        
        " Flux    ",
        "   Meter "
        );    
#endif
    do {
        display_update_int_state_machine();        
    } while (timer_standby);

    timer_standby = 2000;
    SCREEN_ShowText2(
        "Kirno    ",
        "  Intl.  ",
        "    LLC  ",
        "         "        
        );

    do {
        display_update_int_state_machine();        
    } while (timer_standby);
    

    // --- Main Loop
    while (1)
    {
        // go always to meas loop
        Measure_Loop ();        
    }    
    //--- End of Production Program ---

    return 0;
}

//--- End of Main ---//


// Module Functions ------------------------------------------------------------
// void ConfigurationChange (void)
// {
//     unsigned char saved_ok = 0;

//     if (comms_memory_save_flag)
//     {
//         comms_memory_save_flag_reset;
//         while (!Usart2SendVerifyEmpty());
    
//         __disable_irq();
//         saved_ok = Flash_WriteConfigurations((uint32_t *)&mem_conf, sizeof(mem_conf));
//         __enable_irq();                

//         if (saved_ok == FLASH_COMPLETE)
//             Usart2Debug("Memory Saved OK!\n", 1);
//         else
//             Usart2Debug("Memory Error!!!\n", 1);
//     }
// }


void TimingDelay_Decrement(void)
{
    TIM_Timeouts ();

    if (timer_standby)
        timer_standby--;

    Measure_Timer ();
    
}

void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}


//--- end of file ---//


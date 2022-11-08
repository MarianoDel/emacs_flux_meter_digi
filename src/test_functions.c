//------------------------------------------------
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C #########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "stm32g0xx.h"
#include "hard.h"
#include "tim.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "flash_program.h"

#include "screen.h"
#include "i2c.h"

#include <stdio.h>
#include <string.h>



// Externals -------------------------------------------------------------------
extern volatile unsigned char adc_int_seq_ready;
extern volatile unsigned short adc_ch [];
extern volatile unsigned char adc_int_seq_ready;
extern volatile unsigned short timer_standby;


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led (void);
void TF_SW_UP (void);
void TF_SW_DWN (void);
void TF_Usart2_TxRx (void);

void TF_Usart2_Adc_Dma (void);
void TF_Usart2_Batt_Voltage (void);

void TF_I2C_Send_Data (void);
void TF_I2C_Send_Addr (void);
void TF_I2C_Check_Address (void);
void TF_Oled_Screen (void);
void TF_Oled_Screen_Int (void);



// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led ();
    // TF_SW_UP ();
    // TF_SW_DWN ();
    // TF_Usart2_TxRx ();

    // TF_Usart2_Adc_Dma ();
    TF_Usart2_Batt_Voltage ();

    // TF_I2C_Send_Addr ();
    // TF_I2C_Send_Data ();    

    // TF_Oled_Screen ();
    // TF_Oled_Screen_Int ();
    
}


void TF_Led (void)
{
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        Wait_ms(300);
    }
}


void TF_SW_UP (void)
{
    while (1)
    {
        if (SW_UP)
            LED_ON;
        else
            LED_OFF;

        Wait_ms(50);
    }
}


void TF_SW_DWN (void)
{
    while (1)
    {
        if (SW_DWN)
            LED_ON;
        else
            LED_OFF;

        Wait_ms(50);
    }
}


void TF_Usart2_TxRx (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    char s_to_compare [100] = { 0 };
    char s_to_send [] = {"Ready to test...\n"};

    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 1000;
            Usart2Send(s_to_send);
            LED_ON;

        }
        
        if (Usart2HaveData())
        {
            Usart2HaveDataReset();
            Usart2ReadBuffer((unsigned char *) s_to_compare, sizeof(s_to_compare));
            if (!strncmp(s_to_send, s_to_compare, sizeof(s_to_send) - 2))
                LED_OFF;

        }
    }
}


void TF_Usart2_Adc_Dma (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    //-- ADC Init
    AdcConfig();

    //-- DMA configuration and Init
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    unsigned short cntr = 0;
    char s_to_send [100] = { 0 };
    Usart2Send("\nTesting ADC with dma transfers...\n");

    while (1)
    {
        if (sequence_ready)
        {
            sequence_ready_reset;
            if (cntr < 10000)
                cntr++;
            else
            {
                sprintf(s_to_send, "V_Sense_Bat: %d\n",
                        V_Sense_Bat);
                
                Usart2Send(s_to_send);
                cntr = 0;
            }
        }
    }
}


void TF_Usart2_Batt_Voltage (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    //-- ADC Init
    AdcConfig();

    //-- DMA configuration and Init
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    char s_to_send [100] = { 0 };
    unsigned short seq_cnt = 0;
    float volts = 0.0;
    unsigned char volts_int = 0;
    unsigned char volts_dec = 0;

    Usart2Send("\nTesting ADC with dma meas VBatt...\n");
    
    while (1)
    {
        if (sequence_ready)
        {
            sequence_ready_reset;
            seq_cnt++;
            if (seq_cnt > 10000)
            {
                seq_cnt = 0;
                volts = V_Sense_Bat * 3.3 * 2;
                volts = volts / 4095;
                volts_int = (unsigned char) volts;
                volts = volts - volts_int;
                volts = volts * 100;
                volts_dec = (unsigned char) volts;
                sprintf(s_to_send, "vdig: %d voltage: %d.%02dV\n",
                        V_Sense_Bat,
                        volts_int,
                        volts_dec);
                Usart2Send(s_to_send);
            }
        }
    }
}


// Test with I2C without ints
// disable irqs on i2c.h
#ifndef I2C_WITH_INTS
void TF_I2C_Send_Data (void)
{
    I2C1_Init();
    
    while (1)
    {
        LED_ON;
        I2C1_SendByte (I2C_ADDRESS_SLV, 0x55);
        Wait_ms (30);
        LED_OFF;
        Wait_ms(970);
    }
    
}


void TF_I2C_Send_Addr (void)
{
    I2C1_Init();
    
    while (1)
    {
        LED_ON;
        I2C1_SendAddr (I2C_ADDRESS_SLV);
        // I2C1_SendAddr (0x3D);        
        Wait_ms (30);
        LED_OFF;
        Wait_ms(970);
    }
}


void TF_Oled_Screen (void)
{
    // OLED Init
    Wait_ms(500);    //for supply stability
    I2C1_Init();
    Wait_ms(10);

    //primer pantalla
    LED_ON;
    SCREEN_Init();
    LED_OFF;
    
    while (1)
    {
        LED_ON;
        display_contrast (255);        
        SCREEN_ShowText2(
            "Primera  ",
            " Pantalla",
            "         ",
            "         "
            );

        LED_OFF;
        Wait_ms(5000);

        LED_ON;
        display_contrast (10);
        SCREEN_ShowText2(
            "         ",
            "         ",
            "Segunda  ",
            " Pantalla"
            );
        LED_OFF;
        Wait_ms(5000);

        display_contrast (255);        
        LED_ON;
        display_invert(1);
        SCREEN_ShowText2(
            "Third    ",
            "  Screen ",
            "         ",
            "         "
            );
        LED_OFF;
        Wait_ms(5000);

        LED_ON;
        display_invert(0);
        SCREEN_ShowText2(
            "         ",
            "         ",            
            "Forth    ",
            "  Screen "
            );
        LED_OFF;
        Wait_ms(5000);
    }
}
#endif    // I2C without ints


// Test with I2C with ints
// enable irqs on i2c.h
#ifdef I2C_WITH_INTS
extern void display_update_int_state_machine (void);
void TF_Oled_Screen_Int (void)
{
    // OLED Init
    Wait_ms(500);    //for supply stability
    I2C1_Init();
    Wait_ms(10);

    //primer pantalla
    SCREEN_Init();

    unsigned char a = 0;
    while (1)
    {
        if (!timer_standby)
        {
            LED_ON;
            timer_standby = 1000;
            if (a)
            {
                SCREEN_ShowText2(
                    "Primera  ",
                    " Pantalla",
                    "         ",
                    "         "
                    );
                a = 0;
            }
            else
            {
                SCREEN_ShowText2(
                    "         ",
                    "         ",
                    "Segunda  ",
                    " Pantalla"
                    );
                a = 1;
            }
            LED_OFF;
        }
        display_update_int_state_machine();
    }
}
#endif


// void TF_Usart2_Adc_Polling (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart2Config();

//     //-- ADC Init
//     AdcConfig();
//     ADC1->CR |= ADC_CR_ADSTART;

//     unsigned short cntr = 0;
//     char s_to_send [100] = { 0 };
//     Usart2Send("\nTesting ADC with polling...\n");

//     while (1)
//     {
//         if (ADC1->ISR & ADC_ISR_EOC)
//         {
//             ADC1->ISR |= ADC_ISR_EOC;
//             if (cntr < 10000)
//                 cntr++;
//             else
//             {
//                 sprintf(s_to_send, "last ADC: %d\n", (unsigned short) ADC1->DR);
//                 Usart2Send(s_to_send);
//                 cntr = 0;
//             }
//         }
//     }
// }


// void TF_Usart2_Adc_Interrupt (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart2Config();

//     //-- ADC Init
//     AdcConfig();
//     ADC1->CR |= ADC_CR_ADSTART;

//     unsigned short cntr = 0;
//     char s_to_send [100] = { 0 };
//     Usart2Send("\nTesting ADC with Interrupts...\n");

//     while (1)
//     {
//         if (adc_int_seq_ready)
//         {
//             adc_int_seq_ready = 0;
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
            
//             if (cntr < 10000)
//                 cntr++;
//             else
//             {
//                 sprintf(s_to_send, "V_Sense_4V: %d V_Sense_12V: %d\n",
//                         V_Sense_4V,
//                         V_Sense_12V);
//                 Usart2Send(s_to_send);
//                 cntr = 0;
//             }
//         }
//     }
// }




// void TF_Usart2_Flash_Empty_Page (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart2Config();

//     char s_to_send [100] = { 0 };
//     unsigned char * p;
//     p = (unsigned char *) PAGE15_ADDR;
    
//     Usart2Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart2Send(s_to_send);
//         Wait_ms(20);
//     }

//     Usart2Send("\nBlanking flash...\n");
//     Wait_ms(500);
//     if (Flash_ErasePage(FLASH_PAGE_FOR_BKP, 1) == FLASH_COMPLETE)
//     {
//         Usart2Send("Blank OK\n");
//         Wait_ms(100);
//     }
//     else
//     {
//         Usart2Send("Blank NOK\n");
//         Wait_ms(100);
//     }

//     Usart2Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart2Send(s_to_send);
//         Wait_ms(20);
//     }
    
//     while (1)
//     {
//         Wait_ms(300);
//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;

//     }
// }


// void TF_Usart2_Flash_Write_Data (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart2Config();

//     char s_to_send [100] = { 0 };
//     unsigned char * p;
//     p = (unsigned char *) PAGE15_ADDR;
    
//     Usart2Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart2Send(s_to_send);
//         Wait_ms(20);
//     }

//     //write mem conf
//     struct mem_conf_st {
//         uint32_t d0;
//         uint32_t d1;
//         uint32_t d2;
//         uint32_t d3;
//     };

//     struct mem_conf_st mem_conf;
//     mem_conf.d0 = 0x5555;
//     mem_conf.d1 = 0xAAAA;
//     mem_conf.d2 = 0x0000;
//     mem_conf.d3 = 0x7777;

//     Usart2Send("\nWriting Flash...\n");
//     Wait_ms(300);
//     if (Flash_WriteConfigurations((uint32_t *) &mem_conf, sizeof(mem_conf)) == FLASH_COMPLETE)
//         Usart2Send("Seems all good\n");

//     Wait_ms(300);
//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %x %x %x %x %x %x %x %x\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart2Send(s_to_send);
//         Wait_ms(20);
//     }

//     Wait_ms(300);
//     Usart2Send("\nVerifing Flash Backuped Data...\n");
//     Wait_ms(300);

//     struct mem_conf_st mem_backuped;
//     memcpy(&mem_backuped, (uint32_t *) FLASH_ADDRESS_FOR_BKP, sizeof(mem_backuped));

//     if ((mem_conf.d0 == mem_backuped.d0) &&
//         (mem_conf.d1 == mem_backuped.d1) &&
//         (mem_conf.d2 == mem_backuped.d2) &&
//         (mem_conf.d3 == mem_backuped.d3))
//         Usart2Send("Verified OK!!!\n");
//     else
//         Usart2Send("Verified NOK errors in backuped data\n");
        
//     while (1)
//     {
//         Wait_ms(300);
//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;

//     }
// }


// void TF_ReadMemory (void)
// {
//     char s_to_send [100] = { 0 };
//     unsigned char * p;
//     p = (unsigned char *) PAGE15_ADDR;
    
//     Usart2Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 127; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart2Send(s_to_send);
//         Wait_ms(20);
//     }
// }
//--- end of file ---//

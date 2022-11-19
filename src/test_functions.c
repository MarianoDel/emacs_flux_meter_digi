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

#include "spi.h"
#include "tmag_5170.h"

#include "dsp.h"

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

void TF_TMAG_Version (void);
void TF_TMAG_Version_Serial (void);
void TF_TMAG_Z_Serial (void);
void TF_TMAG_Z_Oled (void);
void TF_TMAG_Z_Max_Sample (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    TF_Led ();
    // TF_SW_UP ();
    // TF_SW_DWN ();
    // TF_Usart2_TxRx ();

    // TF_Usart2_Adc_Dma ();
    // TF_Usart2_Batt_Voltage ();

    // TF_I2C_Send_Addr ();
    // TF_I2C_Send_Data ();    

    // TF_Oled_Screen ();
    // TF_Oled_Screen_Int ();

    // TF_TMAG_Version ();
    // TF_TMAG_Version_Serial ();
    // TF_TMAG_Z_Serial ();
    // TF_TMAG_Z_Oled ();
    // TF_TMAG_Z_Max_Sample ();
    
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


void TF_TMAG_Version (void)
{
    unsigned short reg = 0;
    
    Wait_ms(50);    //for supply stability
    
    // init SPI
    SPI_Config ();

    while (1)
    {
        LED_ON;
        Wait_ms (50);
        
        // get tmag version
        reg = TMAG_5170_Read_Register (TEST_CONFIG);

        if (((reg & 0x00F0) == 0x40) ||    //rev A1
            ((reg & 0x00F0) == 0x50))    //rev A2
            LED_ON;
        else
            LED_OFF;

        Wait_ms (1000);
    }
    
}


void TF_TMAG_Version_Serial (void)
{
    char s_to_send [50];
    unsigned short reg = 0;
    
    Wait_ms(50);    //for supply stability

    // init Usart
    Usart2Config();
    
    // init SPI
    SPI_Config ();

    while (1)
    {
        LED_ON;
        Wait_ms (50);
        
        // get tmag version
        reg = TMAG_5170_Read_Register (TEST_CONFIG);

        sprintf(s_to_send, "TEST reg: 0x%04x\n", reg);
        Usart2Send(s_to_send);
        

        if (((reg & 0x00F0) == 0x40) ||    //rev A1
            ((reg & 0x00F0) == 0x50))    //rev A2
            LED_ON;
        else
            LED_OFF;

        Wait_ms (1000);
    }
}


void TF_TMAG_Z_Serial (void)
{
    char s_to_send [50];
    unsigned short reg = 0;
    
    Wait_ms(50);    //for supply stability

    // init Usart
    Usart2Config();
    
    // init SPI
    SPI_Config ();

    // init Tmag config
    TMAG_5170_Init ();

    while (1)
    {
        LED_ON;
        Wait_ms (50);

        unsigned char ready = 0;
        TMAG_5170_Start_Convertion ();        

        do {
            reg = TMAG_5170_Read_Register (CONV_STATUS);
            if ((reg & 0x2000) ||    // conv_rdy bit13
                (reg & 0x0400))    // z_rdy bit10
                ready = 1;
            
        } while (!ready);
        
        // get tmag z-axis
        reg = TMAG_5170_Read_Register (Z_CH_RESULT);

        short twos = reg;
        sprintf(s_to_send, "z-axis: 0x%04x 2s: %d 2s>>4: %d\n",
                reg,
                twos,
                (twos >> 4));
        Usart2Send(s_to_send);
        
        Wait_ms (100);
        int total = TMAG_5170_Convert_Meas_to_B (reg);
        sprintf(s_to_send, "z-axis: %3d Gauss\n", total);
        Usart2Send(s_to_send);
        
        Wait_ms (900);        
    }
}


void TF_TMAG_Z_Oled (void)
{
    char s_to_send [50];
    unsigned short reg = 0;
    unsigned short data = 0;    
    
    // OLED Init
    Wait_ms(500);    //for supply stability
    I2C1_Init();
    Wait_ms(10);

    // first screen
    SCREEN_Init();

    timer_standby = 2000;
    SCREEN_ShowText2(
        "Gausstek ",
        "    Meter",
        "Kirno    ",
        "Intl. LLC"
        );

    do {
        display_update_int_state_machine();        
    } while (timer_standby);
    
    // init Usart
    Usart2Config();
    
    // init SPI
    SPI_Config ();

    // init Tmag config
    //   disable crc
    TMAG_5170_Disable_Crc ();
    //   enable z conversion
    data = (0x0004 << 6);
    TMAG_5170_Write_Register (SENSOR_CONFIG, data);

    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 500;
            unsigned char ready = 0;
            TMAG_5170_Start_Convertion ();        

            do {
                reg = TMAG_5170_Read_Register (CONV_STATUS);
                if ((reg & 0x2000) ||    // conv_rdy bit13
                    (reg & 0x0400))    // z_rdy bit10
                    ready = 1;
            
            } while (!ready);
        
            // get tmag z-axis
            reg = TMAG_5170_Read_Register (Z_CH_RESULT);

            short twos = reg;
        
            int total = (int) twos;
            total = total * 2 * 500;
            total >>= 16;

            sprintf(s_to_send, "z-axis: %3dG\n", total);
            Usart2Send(s_to_send);

            
            sprintf(s_to_send, "Bp: %3dG", total);
            SCREEN_ShowText2(
                s_to_send,
                "         ",
                "         ",
                "         "
                );
        }
        display_update_int_state_machine();
    }
}


void TF_TMAG_Z_Max_Sample (void)
{
    char s_to_send [50];
    unsigned short reg = 0;
    unsigned short data = 0;    
    
    // init Usart
    Usart2Config();
    
    // init SPI
    SPI_Config ();

    // init Tmag config
    //   disable crc
    TMAG_5170_Disable_Crc ();
    //   enable z conversion
    data = (0x0004 << 6);
    TMAG_5170_Write_Register (SENSOR_CONFIG, data);

    int samples_cnt = 0;
    
    while (1)
    {
        unsigned char ready = 0;
        TMAG_5170_Start_Convertion ();        

        do {
            reg = TMAG_5170_Read_Register (CONV_STATUS);
            if ((reg & 0x2000) ||    // conv_rdy bit13
                (reg & 0x0400))    // z_rdy bit10
                ready = 1;
            
        } while (!ready);
        
        // get tmag z-axis
        reg = TMAG_5170_Read_Register (Z_CH_RESULT);

        samples_cnt++;

        if (!timer_standby)
        {
            // one second pass
            sprintf(s_to_send, "samples: %d\n", samples_cnt);
            Usart2Send(s_to_send);
            samples_cnt = 0;
            timer_standby = 1000;
        }
    }
}

//--- end of file ---//

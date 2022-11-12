//------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TMAG_5170.C ##############################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "tmag_5170.h"
#include "spi.h"


// Module Private Types Constants and Macros -----------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned short tmag_range = 500;


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void TMAG_5170_Init (void)
{
    unsigned short reg;
    
    // disable crc
    TMAG_5170_Disable_Crc ();

    // enable z conversion
    reg = (0x0004 << 6);
    TMAG_5170_Write_Register (SENSOR_CONFIG, reg);

    // increase resolution to 2x
    // reg = (0x0001 << 12);
    // TMAG_5170_Write_Register (DEVICE_CONFIG, reg);

    // increase resolution to 4x
    // reg = (0x0002 << 12);
    // TMAG_5170_Write_Register (DEVICE_CONFIG, reg);

    // increase resolution to 8x
    // reg = (0x0003 << 12);
    // TMAG_5170_Write_Register (DEVICE_CONFIG, reg);

    // increase resolution to 16x
    reg = (0x0004 << 12);
    TMAG_5170_Write_Register (DEVICE_CONFIG, reg);
    
    // increase resolution to 32x
    // reg = (0x0005 << 12);
    // TMAG_5170_Write_Register (DEVICE_CONFIG, reg);
    
    // change range to 25mT -- 250G
    // reg = (0x0004 << 6) | (0x0001 << 4);
    // TMAG_5170_Write_Register (SENSOR_CONFIG, reg);
    // tmag_range = 250;

    // change range to 100mT -- 1000G
    // reg = (0x0004 << 6) | (0x0002 << 4);
    // TMAG_5170_Write_Register (SENSOR_CONFIG, reg);
    // tmag_range = 1000;
    
}


void TMAG_5170_Send_Frame (unsigned char * f_to_send, unsigned char * f_getted)
{
    SPI_Chip_Select_On ();
    for (int i = 0; i < 4; i++)
        *(f_getted + i) = SPI_Send_Receive (*(f_to_send + i));

    SPI_Chip_Select_Off ();

}


void TMAG_5170_Disable_Crc (void)
{
    unsigned char send [4] = { 0 };
    unsigned char receiv [4] = { 0 };

    send [0] = 0x0F;
    send [1] = 0x00;
    send [2] = 0x04;
    send [3] = 0x07;    

    TMAG_5170_Send_Frame (send, receiv);
}


unsigned short TMAG_5170_Read_Register (unsigned char reg_to_read)
{
    unsigned char send [4] = { 0 };
    unsigned char receiv [4] = { 0 };
    unsigned short answer = 0;

    send [0] = reg_to_read | 0x80;

    TMAG_5170_Send_Frame (send, receiv);

    answer = receiv[1];
    answer <<= 8;
    answer |= receiv[2];

    return answer;
    
}


void TMAG_5170_Write_Register (unsigned char reg_to_write, unsigned short data)
{
    unsigned char send [4] = { 0 };
    unsigned char receiv [4] = { 0 };

    send [0] = reg_to_write & 0x7F;
    send [1] = (unsigned char) (data >> 8);
    send [2] = (unsigned char) data;

    TMAG_5170_Send_Frame (send, receiv);
}


void TMAG_5170_Start_Convertion (void)
{
    unsigned char send [4] = { 0 };
    unsigned char receiv [4] = { 0 };

    // read conv status reg and start a convertion (cmd0)
    send [0] = CONV_STATUS | 0x80;
    send [3] = 0x10;

    TMAG_5170_Send_Frame (send, receiv);
}


int TMAG_5170_Convert_Meas_to_B (unsigned short meas)
{
    unsigned char negative = 0;
    
    // strip -1
    short twos = (short) meas;
    if (twos < 0)
    {
        negative = 1;
        twos = -twos;        
    }
    
    int total = twos * 2 * tmag_range;    
    total >>= 16;

    if (negative)
        total = -total;

    return total;
}

//--- end of file ---//

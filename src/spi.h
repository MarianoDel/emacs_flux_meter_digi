//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### SPI.H #################################
//---------------------------------------------
#ifndef _SPI_H__
#define _SPI_H__


// Module Exported Types Constants and Macros ----------------------------------


// Module Configurations -------------------------------------------------------
// Uncomment the line below if you will use the SPI peripheral as a Master
#define SPI_MASTER
// #define SPI_SLAVE

// clock when idle
#define SPI_CLK_IDLE_RESET
// #define SPI_CLK_IDLE_SET

//When to validate the Data sended
#define SPI_DATA_VALID_ON_RISING_CLK
// #define SPI_DATA_VALID_ON_FALLING_CLK


//-------- Functions -------------
void SPI_Config(void);
unsigned char SPI_Send_Receive (unsigned char);
void SPI_Busy_Wait (void);
void SPI_Send_Multiple (unsigned char);
void SPI_Send_Single (unsigned char);
unsigned char SPI_Receive_Single (void);
void SPI_Chip_Select_On (void);
void SPI_Chip_Select_Off (void);


#endif    /* _SPI_H__ */

//--- end of file ---//

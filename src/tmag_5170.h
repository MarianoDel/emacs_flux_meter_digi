//------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TMAG_5170.H ##############################
//------------------------------------------------
#ifndef _TMAG_5170_H_
#define _TMAG_5170_H_


// Module Exported Types Constants and Macros ----------------------------------
typedef enum {
    DEVICE_CONFIG = 0x0,  //  Configure Device Operation Modes Go
    SENSOR_CONFIG = 0x1, //   Configure Device Operation Modes Go
    SYSTEM_CONFIG = 0x2, //   Configure Device Operation Modes Go
    ALERT_CONFIG = 0x3, //  Configure Device Operation Modes Go
    X_THRX_CONFIG = 0x4, //   Configure Device Operation Modes Go
    Y_THRX_CONFIG = 0x5, //   Configure Device Operation Modes Go
    Z_THRX_CONFIG = 0x6, //   Configure Device Operation Modes Go
    T_THRX_CONFIG = 0x7, //   Configure Device Operation Modes Go
    CONV_STATUS = 0x8,  //   Conversion Satus Register Go
    X_CH_RESULT = 0x9,  //   Conversion Result Register Go
    Y_CH_RESULT = 0xA,  //   Conversion Result Register Go
    Z_CH_RESULT = 0xB,  //   Conversion Result Register Go
    TEMP_RESULT = 0xC,  //   Conversion Result Register Go
    AFE_STATUS = 0xD,  //   Safety Check Satus Register Go
    SYS_STATUS = 0xE,  //   Safety Check Satus Register Go
    TEST_CONFIG = 0xF,  //   Test Configuration Register Go
    OSC_MONITOR = 0x10,  //   Conversion Result Register Go
    MAG_GAIN_CONFIG = 0x11,  //   Configure Device Operation Modes Go
    ANGLE_RESULT = 0x13,  //   Conversion Result Register Go
    MAGNITUDE_RESULT = 0x14, //  Conversion Result Register Go
    LAST_ADDRESS
    
} tmag_reg_e;
// Config Defines --------------------------------------------------------------


// Common Defines --------------------------------------------------------------
    

// Module Exported Functions ---------------------------------------------------
void TMAG_5170_Init (void);
void TMAG_5170_Disable_Crc (void);
void TMAG_5170_Start_Convertion (void);
void TMAG_5170_Send_Frame (unsigned char * f_to_send, unsigned char * f_getted);
unsigned short TMAG_5170_Read_Register (unsigned char reg_to_read);
void TMAG_5170_Write_Register (unsigned char reg_to_write, unsigned short data);
int TMAG_5170_Convert_Meas_to_B (unsigned short meas);


#endif    /* _TMAG_5170_H_ */

//----------------------------------------------------
// #### PROJECT Digital Flux Meter - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #######################################
//----------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef HARD_H_
#define HARD_H_


//----------- Defines For Configuration -------------

//----------- Hardware Board Version -------------
#define HARDWARE_VER_1_0    // original version

//----------- Firmware Version -------------------
#define FIRMWARE_VER_1_0    // 

//-------- Clock Frequency ------------------------------------
#define CLOCK_FREQ_64_MHZ    //beware change by hand system_stm32g0xx.c SystemInit
// #define CLOCK_FREQ_16_MHZ    //beware change by hand system_stm32g0xx.c SystemInit

//-------- End Of Defines For Configuration ------


#ifdef HARDWARE_VER_1_0
#define HARD    "Hardware version 1.0\n"
#endif


#ifdef FIRMWARE_VER_1_0
#define SOFT    "Firmware version 1.0\n"
#endif

//--------- Sanity Checks ----------
#if (!defined HARDWARE_VER_1_0)
#error "define hardware version on hard.h"
#endif

#if (!defined FIRMWARE_VER_1_0)
#error "define firmware version on hard.h"
#endif


// Gpios Configuration ------------------------------
#ifdef HARDWARE_VER_1_0
//GPIOA pin0
//GPIOA pin1	nc

//GPIOA pin2    usart2 tx
//GPIOA pin3	usart2 rx

//GPIOA pin4    nc

//GPIOA pin5    Spi1 SCK
//GPIOA pin6	Spi1 MISO 
//GPIOA pin7    Spi1 MOSI

//GPIOB pin0
#define CSN    ((GPIOB->ODR & 0x0001) != 0)
#define CSN_ON    (GPIOB->BSRR = 0x00000001)
#define CSN_OFF    (GPIOB->BSRR = 0x00010000)

//GPIOB pin1    nc

//GPIOB pin2    V_Sense_Bat

//GPIOA pin8    nc

//GPIOA pin9    I2C1_SCL

//GPIOC pin6    nc

//GPIOA pin10	I2C1_SDA

//GPIOA pin11    nc

//GPIOA pin12    Led
#define LED    ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00001000)
#define LED_OFF    (GPIOA->BSRR = 0x10000000)

//GPIOA pin13    swdio
//GPIOA pin14    swclk

//GPIOA pin15    SW_UP
#define SW_UP    ((GPIOA->IDR & 0x8000) == 0)

//GPIOB pin3    
//GPIOB pin4
//GPIOB pin5    
//GPIOB pin6
//GPIOB pin7    nc

//GPIOB pin8    SW_DWN
#define SW_DWN    ((GPIOB->IDR & 0x0100) == 0)

//GPIOB pin9    nc
#endif     //#ifdef HARDWARE_VER_1_0




// Exported Types Constants & Macros  ------------------------------------------
//Estados Externos de LED BLINKING
#define LED_NO_BLINKING               0
#define LED_STANDBY                   1
#define LED_GSM_NETWORK_LOW_RSSI      2
#define LED_GSM_NETWORK_HIGH_RSSI     3
#define LED_LOW_VOLTAGE               4
#define LED_GSM_CMD_ERRORS            20

typedef enum
{
    main_init = 0,
    main_wait_for_gsm_network,
    main_ready,
    main_report_alarm_input,
    main_report_panel_input,
    main_report_buffer,
    main_report_buffer_not_sended,
    main_report_buffer_sended,
    main_enable_output,
    main_sms_not_sended

} main_state_t;

typedef enum {
    resp_ok = 0,
    resp_continue,
    resp_selected,
    resp_change,
    resp_change_all_up,
    resp_working,
    resp_error,
    resp_need_to_save,
    resp_finish,
    resp_nok,
    resp_timeout,
    resp_ready,
    resp_no_answer,
    resp_save

} resp_t;


//--- Stringtify Utils -----------------------
#define str_macro(s) #s


// Module Exported Functions ---------------------------------------------------
void ChangeLed (unsigned char);
void UpdateLed (void);

void ChangeLedActivate (unsigned char how_many);
void UpdateLedActivate (void);
void ToggleLedActivate (void);

void Activation_12V_On (void);
void Activation_12V_Off (void);

void WelcomeCode (void);
void HARD_Timeouts (void);
unsigned char Check_Alarm_Input (void);

void Led_Off (void);
void Led_On (void);
unsigned char Led_Status (void);

unsigned char Status_Status (void);
void PwrKey_On (void);
void PwrKey_Off (void);
unsigned char PwrKey_Status (void);
unsigned char NetLight_Status (void);

#endif /* HARD_H_ */

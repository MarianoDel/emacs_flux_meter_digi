//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MEASURE.C #############################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "measure.h"

#include "tmag_5170.h"
#include "dsp.h"
#include "usart.h"

#include "screen.h"
#include "ssd1306_display.h"

#include <stdio.h>
#include <string.h>

// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MEAS_CONFIG,
    MEAS_TAKE_SAMPLES,
    MEAS_SHOW,
    MEAS_PREPAIR_NEXT
    
} measure_states_e;

#define NORTH    0
#define SOUTH    1
// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
measure_states_e meas_state = MEAS_CONFIG;
volatile unsigned short timer_meas = 0;
short max_z = 0;
short min_z = 0;
unsigned char polarity = NORTH;

ma16_u16_data_obj_t z_meas_filter;
char s_to_send[50];
char s_to_send2[20];
char s_to_send3[20];


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
void Measure_Loop (void)
{
    unsigned short reg = 0;
    unsigned short z_filter = 0;    
    unsigned char ready = 0;
    // short twos_comp = 0;
    int total = 0;
    
    
    switch (meas_state)
    {
    case MEAS_CONFIG:
        // init Tmag config
        TMAG_5170_Init ();
        
        MA16_U16Circular_Reset (&z_meas_filter);
        meas_state++;
        break;

    case MEAS_TAKE_SAMPLES:

        TMAG_5170_Start_Convertion ();        

        do {
            reg = TMAG_5170_Read_Register (CONV_STATUS);
            if ((reg & 0x2000) ||    // conv_rdy bit13
                (reg & 0x0400))    // z_rdy bit10
                ready = 1;
            
        } while (!ready);
        
        // get tmag z-axis
        reg = TMAG_5170_Read_Register (Z_CH_RESULT);
        total = TMAG_5170_Convert_Meas_to_B (reg);

        if (max_z < total)
            max_z = total;
        else if (min_z > total)
            min_z = total;

        if (!timer_meas)
            meas_state++;
        
        break;

    case MEAS_SHOW:
        if (max_z > -min_z)
            polarity = NORTH;
        else
        {
            max_z = -min_z;
            polarity = SOUTH;
        }

        z_filter = MA16_U16Circular (&z_meas_filter, max_z);

        if (polarity == NORTH)
            sprintf(s_to_send, "z-axis Bp: %3dG Bpm; %3dG North\n", max_z, z_filter);
        else
            sprintf(s_to_send, "z-axis Bp: %3dG Bpm; %3dG South\n", max_z, z_filter);
        
        Usart2Send(s_to_send);

        sprintf(s_to_send, "Bp:  %3dG", max_z);
        sprintf(s_to_send2, "Bpm: %3dG", z_filter);

        if (max_z > 0)
        {
            if (polarity == NORTH)        
                strcpy(s_to_send3, "   North");
            else
                strcpy(s_to_send3, "   South");
        }
        else
            strcpy(s_to_send3, "         ");

        SCREEN_ShowText2(
            s_to_send,
            s_to_send2,
            "         ",
            s_to_send3
            );

        meas_state++;
        break;

    case MEAS_PREPAIR_NEXT:
        max_z = 0;
        min_z = 0;
        timer_meas = 500;
        meas_state = MEAS_TAKE_SAMPLES;        
        break;
        
    default:
        meas_state = MEAS_CONFIG;
        break;
    }

    // concurrent things
    display_update_int_state_machine();
}


void Measure_Timer (void)
{
    if (timer_meas)
        timer_meas--;
}

//--- end of file ---//

/*
 * File:   specialMode.c
 * Author: Aarushi
 *
 * Created on November 28, 2023, 7:24 PM
 */


#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <math.h>
#include <errno.h>

//These header files names need to be changed depending on what they're called in the rest of the project
//#include "timeDelay.h"
//#include "IOs.h"
//#include "clkChange.h"
//#include "uart.h"


//Special mode needs to be integrated with the rest of the code

// From the unlocked state special mode is entered when PB3 is pressed
// Once in special mode, pressing PB2 sets the timer. It can count from 0 to 59 seconds. During the count down the LED blinks at 0.5s interval.
// After timer is set: 
//  if PB1 is pressed longer than 5s then it resets the timer
//  if PB1 is pressed less than 5s it will pause the timer


#define PB1 PORTAbits.RA2
#define PB2 PORTAbits.RA4
#define PB3 PORTBbits.RB4

#define SPECIAL_MODE 0
#define IDLE_STATE 1
#define SET_TIMER 2
#define RESET_TIMER 3
#define TOGGLE_START_PAUSE 4

//These vars need to be integrated with the rest of the code:
//extern uint8_t CNflag; // Global variable CNflag 
//uint8_t state = DEFAULT; //current state variable, starts with default


uint8_t seconds = 0; // Seconds counter
uint8_t start_pause = 0; // Toggles for start/pause function when PB1 pressed

// this will need to be part of some central IOcheck function or the master while loop that checks all the other states outside of the SPECIAL_MODE
void specialModeFunc(void)
{
    
    
    while (state == SPECIAL_MODE ) 
    {  
        Disp2Dec("ENTERED SPECIAL MODE"); //prints message
        XmitUART2('\r', 1); // next line
        
        if (state == IDLE_STATE) // No PBs are pressed
        {       
            LATBbits.LATB8 = 0; // LED Off
            Idle(); // Save power
        }

        
        else if (state == SET_TIMER) // PB2 pressed
        {
            seconds = seconds + 1; // Increment seconds
            if (seconds > 59) // No more than 59 seconds in one minute
            {
                seconds = 59; // Seconds can't go over 59, set back to 59
                state = IDLE_STATE; // Back to initial state, Led Off and Idle
            }
            // Display seconds increment when pressing PB2
            
            Disp2Dec(seconds); //prints "seconds s"
            Disp2String(" s");
            XmitUART2('\r', 1); // next line

            if(PORTBbits.RA4 == 1) // PB2 no longer being pressed
            {
                state = IDLE_STATE; // Back to initial state, Led Off and Idle

            }
        }

        else if (state == RESET_TIMER) // PB1 pressed for more than 5s
        {
           // Display reseted timer
            Disp2String("00s\r"); // Reset timer 
            seconds = 0; // Reset seconds
            start_pause = 0; // Initialize timer again (paused state)
            state = IDLE_STATE; // Back to initial state, Led Off and Idle

        }


        else if (state == TOGGLE_START_PAUSE) // PB1 pressed for less than 5s
        {

            if (start_pause == 1) // If start 
            {
                // 0.5s delay blinking in LED
                LATBbits.LATB8 = 1;  // LED On
                delay_ms(500); // Calling delay_ms function in timeDelay.c with 
                LATBbits.LATB8 = 0;  // LED Off
                delay_ms(500); // Calling delay_ms function in timeDelay.c with 


                // Displaying Countdown
                XmitUART2('\r', 1); // next line
                //prints "seconds s"
                Disp2Dec(seconds);
                Disp2String(" s");
                XmitUART2('\r', 1); // next line
                

                if (start_pause == 0) // If paused
                {
                    pb_state = 0; // Back to initial state, Led Off and Idle

                }

            }
        }
    }
     return;
}  
        

void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
  
    if(IFS1bits.CNIF ==1){  
        
        
        uint16_t seconds_counter = 0; //counter to see how long PB1 is pressed

        if((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 0) && (PORTAbits.RA4 == 1)) // PB3 pressed
        {
            state = SPECIAL_MODE; //enter special mode
        }
            
        else if ((PORTAbits.RA2 == 1) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 0)) // PB2 pressed
        {
            state = SET_TIMER; //set timer to count from 0 to 60 sec
        }
        
        while ((PORTAbits.RA2 == 0) && (PORTBbits.RB4 == 1) && (PORTAbits.RA4 == 1)) // While PB1 pressed 
        {
              
            seconds_counter = seconds_counter + 1; // Count the seconds PB1 is being pressed
                                   
                
            if ((PORTAbits.RA2 == 1) && (seconds_counter >= 5)) // If PB1 is pressed for more than 5s
          
            {
              state = RESET_TIMER; 
                   
            } 
                
            else if ((PORTAbits.RA2 == 1) && (seconds_counter  < 5))  // If PB1 is pressed for less than 5s
            { 
 
                if (start_pause == 0) // Toggle each time PB1 is pressed for less than 5s (one for start, one for pause)
                {
                    start_pause = 1;

                    XmitUART2('\r', 1);   
                    Disp2String("Start timer"); // Displays message
                    XmitUART2('\r', 1); // next line
                    XmitUART2(' ', 10);  // spaces                         
                    XmitUART2('\r', 1); // next line

                    state = TOGGLE_START_PAUSE; // PB1 pressed for less than 5s

                }
                else
                {

                    start_pause = 0;

                    XmitUART2('\r', 1);
                    Disp2String("Stop timer"); // Displays message
                    XmitUART2('\r', 1); // next line
                    XmitUART2(' ', 10);    //spaces   
                    XmitUART2('\r', 1);  // next line

                    state = IDLE_STATE;// Back to initial state after timer being paused

                }
                 
            }
  
        }
  
         
    }
    
    IFS1bits.CNIF = 0;  // Clear IF flag

}
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

#define LED_ON LATBbits.LATB8=1
#define LED_OFF LATBbits.LATB8=0
#define LED_BLINK LATBbits.LATB8^=1

#define PB1 PORTAbits.RA2
#define PB2 PORTAbits.RA4
#define PB3 PORTBbits.RB4

#define SPECIAL_MODE 0
#define IDLE_STATE 1
#define SET_TIMER 2
#define RESET_TIMER 3
#define TOGGLE_START_PAUSE 4

uint8_t state= SPECIAL_MODE;                  //Start in SPECIAL_MODE
uint8_t count=3;                              //Multiple button push
uint8_t CNflag=0;                             //CN interrupt flag
uint8_t p1=1;                                 //PB1
uint8_t p2=1;                                 //PB2
uint8_t p3=1;                                 //PB3
uint8_t first=1;                              //Used to determine first time in loop


uint8_t seconds = 0; // counts seconds on timer
uint16_t seconds_counter = 0; //counter to see how long PB1 is pressed
uint8_t start_pause = 0; // Toggles for start/pause function when PB1 pressed

void IOcheck(void) {
    //read all values
    delay_ms2(50);                        //delay that cant be interrupted by PB press for proper de-bounce
    p1=PB1;
    p2=PB2;
    p3=PB3;
    count=p1+p2+p3;                      //Used to see if multiple push buttons are pressed
    while (p1 == 0) // While PB1 pressed 
    {
        seconds_counter = seconds_counter + 1; // Count the seconds PB1 is being pressed

        if ((p1 == 1) && (seconds_counter  < 5))  // If PB1 is pressed for less than 5s
        { 

            if (start_pause == 0) // Toggle each time PB1 is pressed for less than 5s (one for start, one for pause)
            {
                start_pause = 1;
            }
            else
            {
                start_pause = 0;   
            }
        }
    }    
}


// this will need to be part of the master while loop that checks all the other states outside of the SPECIAL_MODE
void specialModeFunc(void)
{
   while(1){ 
    //Pressing PB3 will make system enter special mode.
        while (state == SPECIAL_MODE ) 
        {  
            LED_OFF;

            if(first==1){
                Disp2Dec("ENTERED SPECIAL MODE"); //prints message
                XmitUART2('\r', 1); // next line
                first=0;//clears first since it is no longer the first cycle
            }
            Idle(); //Wait for interrupt
            IOcheck();//Check IO

            if(p2==0 && count==2){ //If PB2 is pressed
                first=1;
                state=SET_TIMER;
            }
            
            else if(p1==0 && count==2){ //If PB1 is pressed
                if(p1==1 && seconds_counter>=5){ //PB1 is pressed for 5s or more
                    first=1;
                    state= RESET_TIMER;
                }

                else if(p1==1 && seconds_counter<5){ //PB1 is pressed for less than 5s
                    first=1;
                    state= TOGGLE_START_PAUSE;
                }
            }
        }    


        while (state == SET_TIMER) // PB2 pressed
        {
            if(first==1){
                seconds = seconds + 1; // Increment seconds
                first=0;
                if (seconds > 59) // No more than 59 seconds in one minute
                {
                    seconds = 59; // Seconds can't go over 59, set back to 59
                }
                // Display seconds increment when pressing PB2

                Disp2Dec(seconds); //prints "seconds s"
                Disp2String(" s");
                XmitUART2('\r', 1); // next line
            }
            Idle();                           //Wait for interrupt
            if(CNflag==1){
                CNflag=0;                     //clear flag
                IOcheck();                    //Update IO read  
                if(p2==0){                    //check for PB2
                    first=1;                  //Makes it so the first if statement is read again in SPECIAL_MODE
                    state=SPECIAL_MODE;       //switch states
                }
            }
        }

        while (state == RESET_TIMER) // PB1 pressed for more than 5s
        {   
            if(first==1){
            first=0;
            // Display reseted timer
            Disp2String("00s\r"); // Reset timer 
            seconds = 0; // Reset seconds
            start_pause = 0; // Initialize timer again (paused state)
            }
            
            Idle();                           //Wait for interrupt
            if(CNflag==1){
                CNflag=0;                     //clear flag
                IOcheck();                    //Update IO read  
                if(p1==0){                    //check for PB1
                    first=1;                  //Makes it so the first if statement is read again in SPECIAL_MODE
                    state=SPECIAL_MODE;       //switch states
                }
            }

        }


        while (state == TOGGLE_START_PAUSE) // PB1 pressed for less than 5s
        {
            if(first==1){
                first= 0;
                if (start_pause == 1) // If start 
                {
                    // 0.5s delay blinking in LED
                    LED_ON;
                    delay_ms(500); // Calling delay_ms function in timeDelay.c with 
                    LED_OFF;  // LED Off
                    delay_ms(500); // Calling delay_ms function in timeDelay.c with 

                    // Displaying Countdown
                    XmitUART2('\r', 1); // next line
                    //prints "seconds s"
                    Disp2Dec(seconds);
                    Disp2String(" s");
                    XmitUART2('\r', 1); // next line

                    if (start_pause == 0) // If paused
                    {
                       state = SPECIAL_MODE; // Back to initial state, Led Off and Idle

                    }
            
                }
            }
            
            Idle();                           //Wait for interrupt
            if(CNflag==1){
                CNflag=0;                     //clear flag
                IOcheck();                    //Update IO read  
                if(p1==0){                    //check for PB1
                    first=1;                  //Makes it so the first if statement is read again in SPECIAL_MODE
                    state=SPECIAL_MODE;       //switch states
                }
            }
        }
     return;
}  
        

void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void)
{
  
    if(PB1==0||PB2==0||PB3==0){//any press of a button will set the flag
        CNflag=1;
    }
    IFS1bits.CNIF = 0; // clear IF flag
    return;

}

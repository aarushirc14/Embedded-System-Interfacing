/*
 * File:   main.c
 * Author: Aarushi Roy Choudhury
 * UCID: 30113987
 *
 * Created on September 19, 2023, 12:37 PM
 * Goal: Turn on the number of LEDs corresponding to your UCID digit 
 *       based on the input (position of UCID digit).
 */


#include "xc.h"


int main(void) {
    //set 4 inputs
    TRISAbits.TRISA0 = 1;   //sets RA0 as input
    TRISAbits.TRISA1 = 1;   //sets RA1 as input
    TRISAbits.TRISA4 = 1;   //sets RA4 as input
    TRISAbits.TRISA5 = 1;   //sets RA5 as input

    //set 8 outputs
    TRISBbits.TRISB0 = 0;   //sets RB0 as output
    TRISBbits.TRISB1 = 0;   //sets RB1 as output
    TRISBbits.TRISB2 = 0;   //sets RB2 as output
    TRISBbits.TRISB4 = 0;   //sets RB4 as output
    TRISBbits.TRISB7 = 0;   //sets RB7 as output
    TRISBbits.TRISB8 = 0;   //sets RB8 as output
    TRISBbits.TRISB9 = 0;   //sets RB9 as output
    TRISBbits.TRISB12 = 0;  //sets RB12 as output
    TRISBbits.TRISB13 = 0;  //sets RB13 as output
    
    while(1){
        if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x0  && PORTAbits.RA1 == 0x0 && PORTAbits.RA0 == 0x1 ){        //0001 then turn 3 LEDs on
           LATB = 0x7;
        }
        else if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x0  && PORTAbits.RA1 == 0x1 && PORTAbits.RA0 == 0x0 ){   //0010 then turn 0 LEDs on
            LATB = 0x0;
        }
        else if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x0  && PORTAbits.RA1 == 0x1 && PORTAbits.RA0 == 0x1 ){   //0011 then turn 1 LEDs on
            LATB = 0x1;
        }
        else if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x1  && PORTAbits.RA1 == 0x0 && PORTAbits.RA0 == 0x0 ){   //0100 then turn 1 LEDs on
            LATB = 0x1;
        }
        else if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x1  && PORTAbits.RA1 == 0x0 && PORTAbits.RA0 == 0x1 ){   //0101 then turn 3 LEDs on
            LATB = 0x7;
        }
        else if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x1  && PORTAbits.RA1 == 0x1 && PORTAbits.RA0 == 0x0 ){   //0110 then turn 9 LEDs on
           LATB = 0x3397; // Binary: 0011 0011 1001 0111
        }
        else if (PORTAbits.RA5 == 0x0 && PORTAbits.RA4 == 0x1  && PORTAbits.RA1 == 0x1 && PORTAbits.RA0 == 0x1 ){   //0111 then turn 8 LEDs on
            LATB = 0x1397;
        }
        else if (PORTAbits.RA5 == 0x1 && PORTAbits.RA4 == 0x0  && PORTAbits.RA1 == 0x0 && PORTAbits.RA0 == 0x1 ){   //1001 then turn 7 LEDs on
            LATB = 0x0397; // Binary: 0000 0011 1001 0111. 
        }
        else{                                                                                                       //for 0000 and 1010-1111 turn no LEDs on
            LATB = 0x0000;
        }
    }
    return 0;
}

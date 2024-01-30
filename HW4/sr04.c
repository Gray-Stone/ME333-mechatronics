#include "sr04.h"
#include <xc.h>

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// initialize the pins used by the SR04
void SR04_Startup(){
    ANSELA = 0; // turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0; // initialize TRIG to LOW
}

// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout){
    // turn on TRIG 
    // wait at least 10us
    // turn off TRIG
    // wait until ECHO is on
    // note the value of the core timer
    // wait until ECHO is off or timeout core ticks has passed
    // note the core timer
    // return the difference in core times
}

float SR04_read_meters(){
    // read the raw rs04 value
    // convert the time to meters, the velocity of sound in air is 346 m/s
    // return the distance in meters
}
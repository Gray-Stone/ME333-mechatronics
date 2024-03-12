#include <xc.h>
#include "nu32dip.h"

#include "current_control.h"


void HBridgeSetup(){
      TRISBCLR = 0b11 << 6 ; // Set PB 6 and 7 to output
    Timer3_OC1_Setup_20khz();
}


void Timer3_OC1_Setup_20khz() {
// OC1 with RPB7, Pin16 as output PWM, with Timer2. 

// Then PB6, Pin 15 as direction.

  RPB7R = 0b0101; // Setup OC1 to output to PB7

  PR3 = 2399;            // set period to 48M / 2400 = 20k
  TMR3 = 0;              // clear count
  T3CONbits.TCS = 0;     // use PBCLK
  T3CONbits.TCKPS = 0;   // prescale of 1
  OC1CONbits.OCFLT = 0;  // Turn off fault
  OC1CONbits.OCTSEL = 1; // Set clock source to timer 3
  OC1CONbits.OCM =
      0b110;    // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1800; // comp 1 set to 24000 * 0.75 = 18000
  T3CONbits.ON = 1;  // Turn on timer 3
  OC1CONbits.ON = 1; // turn on OC1
}

void Timer2Setup5khz() {
  T2CONbits.TCS = 0;      // use PBCLK
  T2CONbits.TCKPS = 0b00; // set prescaler to 0
  // 48MHz / 5k = 9.6k
  PR2 = 9600;         // Select limit.
  TMR2 = 0;            // Clear value.
  T2CONbits.TGATE = 0; // not gated input (the default)
  T2CONbits.TCS = 0;   // PCBLK input (the default)

  // T2 – Timer2 9  8  IFS0<9>  IEC0<9>  IPC2<4:2>  IPC2<1:0>
  IPC2bits.T2IP = 5; // priority 5
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0; // clear interrupt flag
  IEC0bits.T2IE = 1; // enable timer interrupt.

  T2CONbits.ON = 1; // timer on
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
    static int dir = 0 ;
    dir ^= 1;
    MOTOR_DIR ^= 1 ; 
    NU32DIP_YELLOW ^= 1;
    OC1RS = 600 + 1200 * dir; 
  IFS0bits.T2IF = 0;
}

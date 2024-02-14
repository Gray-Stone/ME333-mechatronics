#include "nu32dip.h"

#define MAX_MESSAGE_LENGTH 100

int main() {

  NU32DIP_Startup();
  // Disable interrupt during setup
  __builtin_disable_interrupts();


  PR3 = 2399; // set period to 48M / 2400 = 20k
  TMR3 = 0;    // clear count

  T3CONbits.TCS = 0;     // use PBCLK
  T3CONbits.TCKPS = 0;   // prescale of 1
  OC1CONbits.OCFLT = 0;  // Turn off fault
  OC1CONbits.OCTSEL = 1; // Set clock source to timer 3

  OC1CONbits.OCM =
      0b110; // PWM mode without fault pin; other OC1CON bits are defaults

  OC1RS = 1800; // comp 1 set to 24000 * 0.75 = 18000
  // The LED is connected to PB15, Thus we set output PPS to this pin

  RPB15R = 0b0101;

  T3CONbits.ON = 1; // Turn on timer 3

  OC1CONbits.ON = 1; // turn on OC1
  __builtin_enable_interrupts();

  char message[MAX_MESSAGE_LENGTH];
  sprintf(message, "Value of DEVCFG1  %x \n\r", DEVCFG1);

  NU32DIP_WriteUART1(message);
  // This output  0xff74cf59
  // Which is 1111 1111 0111 0100 1100 1111 0101 1001
  //          31        23        15        7  
  // According to data sheet, FPBDIV is at bit13:12 
  // Which have value 00, so the FPBDIV is 0.

  while (1) {
  }
  return 0;
}
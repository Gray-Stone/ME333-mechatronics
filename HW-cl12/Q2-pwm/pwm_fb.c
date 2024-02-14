#include "nu32dip.h"

#define MAX_MESSAGE_LENGTH 100
char message[MAX_MESSAGE_LENGTH];

#define NUMSAMPS 1000
 // number of points in waveform
static volatile int Waveform[NUMSAMPS];

void makeWaveform() {
  int i = 0, center = 1200, A = 1200; // square wave, fill in center value and
  // amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if (i < NUMSAMPS / 2) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

void Timer2Setup(){
  T2CONbits.TCS = 0;     // use PBCLK
  T2CONbits.TCKPS = 0b00; // set prescaler to 0
  // 48MHz / 1k = 48K
  PR2 = 48000; // Select limit.
  TMR2 = 0;  // Clear value.
  T2CONbits.TGATE = 0; // not gated input (the default)
  T2CONbits.TCS = 0; // PCBLK input (the default)

  // T2 – Timer2 9  8  IFS0<9>  IEC0<9>  IPC2<4:2>  IPC2<1:0>
  IPC2bits.T2IP = 2; // priority 5
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0; // clear interrupt flag
  IEC0bits.T2IE = 1; // enable timer interrupt.

  T2CONbits.ON = 1; // timer on


}
// We cannot use shadow register since our chip doesn't have it
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
  static int counter = 0;
  NU32DIP_GREEN = !NU32DIP_GREEN;
  // initialize counter once
  // Change duty cycle by setting OC1RS
  OC1RS = Waveform[counter];
  counter++;
  // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0;
    // roll the counter over when needed
  }

  // For debugging the output
  // sprintf(message, "Counter %d Waveform[counter] %d OC1RS %d \n\r", counter,
  //         Waveform[counter], OC1RS);
  // NU32DIP_WriteUART1(message);

  IFS0bits.T2IF = 0;
}

int main() {

  NU32DIP_Startup();
  makeWaveform();
  // Disable interrupt during setup
  __builtin_disable_interrupts();

  // The LED is connected to PB15, Thus we set output PPS to this pin
  RPB15R = 0b0101; // Setup OC1 to output to PB15

  PR3 = 2399; // set period to 48M / 2400 = 20k
  TMR3 = 0;    // clear count
  T3CONbits.TCS = 0;     // use PBCLK
  T3CONbits.TCKPS = 0;   // prescale of 1
  OC1CONbits.OCFLT = 0;  // Turn off fault
  OC1CONbits.OCTSEL = 1; // Set clock source to timer 3
  OC1CONbits.OCM =
      0b110; // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1800; // comp 1 set to 24000 * 0.75 = 18000
  T3CONbits.ON = 1; // Turn on timer 3
  OC1CONbits.ON = 1; // turn on OC1

  Timer2Setup();
  __builtin_enable_interrupts();

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
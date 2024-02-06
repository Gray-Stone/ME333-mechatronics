#include "nu32dip.h"


unsigned int last_isr_time = 0;

const unsigned int count_10ms = 2400;

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR

  if ((_CP0_GET_COUNT() - last_isr_time)< count_10ms){
  // Basically skip this round of handing if the interrupt happend 
  // within 10ms of last recorded case.
  // We still update this. So the switch much to have a true 10ms 
  // quiet period 
    last_isr_time = _CP0_GET_COUNT();
    return ;
  }

  // Normal case, also record the timing. 
  last_isr_time = _CP0_GET_COUNT();
  NU32DIP_YELLOW = 0;                  // LED1 and LED2 on
  NU32DIP_GREEN = 0;
  _CP0_SET_COUNT(0);
  
  while(_CP0_GET_COUNT() < 10000000) { ; } // delay for 10 M core ticks, 0.25 s
  
  NU32DIP_YELLOW = 1;                  // LED1 and LED2 off
  NU32DIP_GREEN = 1;
  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts


  // Our button is Connected to Pin12 / A4 
  // But the INT0 is on RB7. so re-wire needed.
  // OR we change to INT2 which is on A4 with INT2R<3:0>=0010

  // Does INT0 not needing any config to work? Seems like it. 
  // A wire to P16 make things work.


  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}

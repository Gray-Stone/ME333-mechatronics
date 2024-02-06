#include "nu32dip.h" 

#define MAX_MESSAGE_LENGTH 100


void Int2Setup(){

  // INT2 – External Interrupt 2 13 11 IFS0<13> IEC0<13> IPC2<28:26> IPC2<25:24>

  // Need to use PPS to select INT2's pin
  // Since Usr btm is already at RPA4, We will simply set INT2R to 0010 to use that.

  TRISASET = 0b010000; // A4 user button

  INT2R = 0b0010;

  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT2EP = 0;          // step 3: INT2 triggers on falling edge
  IPC2bits.INT2IP = 6;            // step 4: interrupt priority 6
  IPC2bits.INT2IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT2IF = 0;            // step 5: clear the int flag
  IEC0bits.INT2IE = 1;            // step 6: enable
  __builtin_enable_interrupts();  // step 7: enable global interrupts

}

unsigned int last_isr_time = 0;

const unsigned int count_10ms = 2400;

// We define a few value of state 
// 2 and 3 is timing mode (stopwatch started) 2 is when it just enter timing mode
// 0 is idle stat. Do nothing.
// 1 is just finished state. remind the main loop to print something.
volatile int state = 0;
volatile unsigned int duration =0 ; // We are using clearing global timer style.

// We cannot use shadow register since our chip doesn't have it 
void __ISR(_EXTERNAL_2_VECTOR, IPL2SOFT) Ext0ISR(void) {

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

  // User push just started the stopwatch
  if (state <1){
    NU32DIP_YELLOW = 0;  // LED1 and LED2 on
    NU32DIP_GREEN = 0;
    _CP0_SET_COUNT(0);
    state = 2;
  } else if (state >1 ){
  // User pushed again to set the timer off
    // LED off
    duration = _CP0_GET_COUNT();
    
    NU32DIP_YELLOW = 1;
    NU32DIP_GREEN = 1;

    state = 1 ; 
  }

  // if state == 1 stop watch finished, but we want the print to finish to screen, 
  // so actually ignore user pushes intentionally.
  
  IFS0bits.INT2IF = 0; // clear the int flag
}



int main (){
    
    NU32DIP_Startup();


    char message[MAX_MESSAGE_LENGTH];
    sprintf(message , "Press the USER button to start the timer");
    NU32DIP_WriteUART1(message);

    while(1){

      if (state == 2){
        // Print the starting message
          sprintf(message, "Press the USER button again to stop the timer");
        NU32DIP_WriteUART1(message);
      
        state = 3;
      } else if (state == 3){
        // Update user with current status
        float current_lasted_s = _CP0_GET_COUNT()/ 24000000.0;
        sprintf(message, "Timer elasped for %f",current_lasted_s);
        NU32DIP_WriteUART1(message);

        unsigned int wait_start;
        while (_CP0_GET_COUNT() - wait_start < 2400000){
            // busy waiting for 0.05 sec 
        }
      } else if (state ==1 ){
        // Just finished timing, print finish message

        unsigned int local_duration = duration; // Copy to prevent race condition
        float print_duraiton_s = local_duration / 24000000.0;
        sprintf(message, "%f seconds elapsed" , print_duraiton_s);
      NU32DIP_WriteUART1(message);

        state ==0; 
      }
    }
    return 0;
}
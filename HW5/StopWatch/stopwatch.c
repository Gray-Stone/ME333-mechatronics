#include "nu32dip.h"

#define MAX_MESSAGE_LENGTH 100

void IntSetup() {
  // INT2 – External Interrupt 2 13 11 IFS0<13> IEC0<13> IPC2<28:26> IPC2<25:24>

  // Need to use PPS to select INT2's pin
  // Since Usr btm is already at RPA4, We will simply set INT2R to 0010 to use
  // that.
  __builtin_disable_interrupts(); // step 2: disable interrupts
  TRISASET = 0b010000;            // A4 user button
  INT2R = 0b0010;                 // PPS INT2 for RP4A
  // IFS0<13> IEC0<13> IPC2<28:26> IPC2<25:24>
  IPC2CLR = 0x1F << 24;
  // Chip having problem handling priority higher then 2.
  // So setting 2 instead of 6.
  IPC2SET = 2 << 26;
  IFS0CLR = 1 << 13;
  IEC0SET = 1 << 13;
  __builtin_enable_interrupts(); // step 7: enable global interrupts
}

unsigned int last_isr_time = 0;

const unsigned int debounce_duration = 2400000 * 5; // 0.5 sec debounce

// We define a few value of state
// 2 and 3 is timing mode (stopwatch started) 2 is when it just enter timing
// mode 0 is idle stat. Do nothing. 1 is just finished state. remind the main
// loop to print something.
volatile int state = 0;
volatile unsigned int duration = 0; // We are using clearing global timer style.

// We cannot use shadow register since our chip doesn't have it
void __ISR(_EXTERNAL_2_VECTOR, IPL2SOFT) Ext0ISR(void) {

  if ((_CP0_GET_COUNT() - last_isr_time) < debounce_duration) {
    // Basically skip this round of handing if the interrupt happend
    // within 10ms of last recorded case.
    // We still update this. So the switch much to have a true 10ms
    // quiet period
    last_isr_time = _CP0_GET_COUNT();
    IFS0CLR = 1 << 13;
    return;
  }
  // Normal case, also record the timing.
  last_isr_time = _CP0_GET_COUNT();

  // User push just started the stopwatch
  if (state < 1) {
    NU32DIP_YELLOW = 0; // LED1 and LED2 on
    NU32DIP_GREEN = 0;
    _CP0_SET_COUNT(0);
    state = 2;
  } else if (state > 1) {
    // User pushed again to set the timer off
    // LED off
    duration = _CP0_GET_COUNT();
    NU32DIP_YELLOW = 1;
    NU32DIP_GREEN = 1;
    state = 1;
  }
  // if state == 1 stop watch finished, but we want the print to finish to
  // screen, so actually ignore user pushes intentionally.

  IFS0CLR = 1 << 13; // clear the interrupt flag.
}

int main() {

  NU32DIP_Startup();
  IntSetup();

  char message[MAX_MESSAGE_LENGTH];
  sprintf(message, "Press the USER button to start the timer\n\r");
  NU32DIP_WriteUART1(message);
  while (1) {

    if (state == 2) {
      // Print the starting message
      sprintf(message, "Press the USER button again to stop the timer\n\r");
      NU32DIP_WriteUART1(message);
      state = 3;
    } else if (state == 3) {
      // Update user with current status
      float current_lasted_s = _CP0_GET_COUNT() / 24000000.0;
      sprintf(message, "Timer elasped for %f\n\r", current_lasted_s);
      NU32DIP_WriteUART1(message);

      unsigned int wait_start = _CP0_GET_COUNT();
      while ((_CP0_GET_COUNT() - wait_start) < 2400000) {
        // busy waiting for 0.05 sec
      }
    } else if (state == 1) {
      // Just finished timing, print finish message
      unsigned int local_duration = duration; // Copy to prevent race condition
      float print_duraiton_s = local_duration / 24000000.0;
      sprintf(message, "%f seconds elapsed\n\n\r", print_duraiton_s);
      NU32DIP_WriteUART1(message);
      sprintf(message, "Press the USER button to start the timer\n\r");
      NU32DIP_WriteUART1(message);

      state = 0;
    }
  }
  return 0;
}
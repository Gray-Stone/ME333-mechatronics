
#include "nu32dip.h"

#include "current_control.h"
#include "encoder.h"
#include "ina219.h"
#include "utilities.h"
#include <stdarg.h>
#include "current_control.h"


#define MAX_MESSAGE_LENGTH 100
char message[MAX_MESSAGE_LENGTH];
char message_in[MAX_MESSAGE_LENGTH];

void PrintUART1(const char *format, ...) {
  // https://stackoverflow.com/questions/42131753/wrapper-for-printf
  va_list args;
  va_start(args, format);
  vsnprintf(message, MAX_MESSAGE_LENGTH, format, args);

  va_end(args);

  NU32DIP_WriteUART1(message);
}

int ReadEncoder();

double Encoder2Deg(int count);

int main() {

  NU32DIP_Startup();
  NU32DIP_GREEN = 0;
  NU32DIP_YELLOW = 0;

  // Encoder stuff
  UART2_Startup();
  set_encoder_flag(0);
  INA219_Startup();

  // Disable interrupt during setup
  __builtin_disable_interrupts();
  // Peripheral setup
  HBridgeSetup();
  Timer2Setup5khz();

  __builtin_enable_interrupts();

  NU32DIP_GREEN = 0;
  NU32DIP_YELLOW = 1;

  set_state(s_IDLE);

  PrintUART1("Main started");

  while (1) {

    NU32DIP_ReadUART1(message, sizeof(message));
     NU32DIP_GREEN = 0;
      NU32DIP_YELLOW = 0;

    char input = message[0];
    switch (input) {
      // A is not an option for NU32DIP
    case 'b': {
      // Read current sensor mA
      double current = INA219_read_current();
      PrintUART1("%f\r\n", current);
      break;
    }
    case 'c': {
      PrintUART1("%d\r\n", ReadEncoder());
      break;
    }
    case 'd': {
      PrintUART1("%f\r\n", Encoder2Deg(ReadEncoder()));
      break;
    }
    case 'e': {
      PrintUART1("Resetting encoder\r\n");
      WriteUART2("b");
      break;
    }
    case 'r': {
      PrintUART1("%d\r\n", get_state());
      break;
    }
    case 'f':{
      // PrintUART1("Getting value");
      NU32DIP_ReadUART1(message_in, sizeof(message_in));
      int temp_pwm;
      sscanf(message_in ,"%d", &temp_pwm);
      PrintUART1("got value : %d \r\n",temp_pwm);
      pwm_value_g =temp_pwm;
      set_state(s_PWM);
      break;
    }
    case 'p': {
      set_state(s_IDLE);
      break;
    }

    case 'q': {
      set_state(s_IDLE);

      NU32DIP_GREEN = 1;
      NU32DIP_YELLOW = 1;

      break;
    }
    default: {
      NU32DIP_GREEN = 0;
      NU32DIP_YELLOW = 0;
      break;
    }
    }
  }
  return 0;
}

int ReadEncoder() {
  WriteUART2("a");
  while (!get_encoder_flag()) {
    // Busy waiting.
  }
  set_encoder_flag(0);

  return get_encoder_count();
}

double Encoder2Deg(int count) {
  // Encoder have 96 line.
  // Pico gives 384 count per rotation
  // (96*4) / (360) deg

  return ((double)count) / 384.0 * 360.0;
}
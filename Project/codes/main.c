
#include "nu32dip.h"

#include "current_control.h"
#include "encoder.h"
#include "ina219.h"
#include "utilities.h"
#include <stdarg.h>
#include "current_control.h"
#include "position_control.h"

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


int main() {

  NU32DIP_Startup();
  NU32DIP_GREEN = 0;
  NU32DIP_YELLOW = 0;

  // Encoder stuff
  UART2_Startup();
  set_encoder_flag(0);
  INA219_Startup();

  // Clear encoder on boot
  WriteUART2("b");


  // Disable interrupt during setup
  __builtin_disable_interrupts();
  // Peripheral setup
  HBridgeSetup();
  Timer2Setup5khz();
  PosCtlSetup();

  __builtin_enable_interrupts();

  NU32DIP_GREEN = 0;
  NU32DIP_YELLOW = 1;

  set_state(s_IDLE);

  PrintUART1("Main started\r\n");

  while (1) {

    NU32DIP_ReadUART1(message_in, sizeof(message_in));
    NU32DIP_GREEN = 0;
    NU32DIP_YELLOW = 1;

    char input = message_in[0];
    // char input;
    // sscanf(message_in, "%c" , input);
    switch (input) {
      // A is not an option for NU32DIP
    case 'b': {
      // Read current sensor mA
      float current = INA219_read_current();
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
    case 'g': {
      // Set current gain
      NU32DIP_ReadUART1(message_in, sizeof(message_in));
      sscanf(message_in, "%f %f", &current_pgain_g, &current_igain_g);
      PrintUART1("got value :p %f i %f\r\n", current_pgain_g, current_igain_g);
      break;
    }
    case 'h': {
      // Get current gain
      PrintUART1("p %f i %f\r\n", current_pgain_g, current_igain_g);
      break;
    }
    case 'k':{
      // Test current control

       NU32DIP_GREEN = 1;

      set_state(s_ITEST);
      // Wait until its done
      while (get_state() != s_IDLE) {
      }

      PrintUART1("%d\r\n", ITEST_PLOTPTS);
      for (int i = 0; i < ITEST_PLOTPTS; i++) {
        // send plot data to MATLAB
        // when first number sent = 1, MATLAB knows we’re done
        sprintf(message, "%d %d\r\n", IMSRarray[i], IREFarray[i]);
        NU32DIP_WriteUART1(message);
      }

      break;
    }

    case 'i':{

      NU32DIP_ReadUART1(message_in, sizeof(message_in));
      sscanf(message_in, "%f %f %f", &position_p_g, &position_i_g, &position_d_g);
      PrintUART1("got value :p %f i %f d %f\r\n", position_p_g, position_i_g, position_d_g);
      break;
    }

    case 'j':{

      PrintUART1("pos ctl :p %f i %f d %f\r\n", position_p_g, position_i_g, position_d_g);
      break;
    }

    case 'l':{
      // Position holding 
      NU32DIP_ReadUART1(message_in, sizeof(message_in));
      float temp_angle ; 
      sscanf(message_in, "%f", &temp_angle);
      target_encoder_g = Deg2Encoder(temp_angle);
      PrintUART1("Set target ang %f , target encoder %d\r\n", temp_angle , target_encoder_g);
      set_state(s_HOLD);
      break;
    }
    case 'n':
    case 'm':{
      // Read the trajectory
      NU32DIP_ReadUART1(message_in, sizeof(message_in));
      sscanf(message_in, "%d", &ref_traj_size);
      NU32DIP_GREEN = 1;
      NU32DIP_YELLOW = 0;
      int i =0;
      float ref_ang;
      for ( i=0 ; i<ref_traj_size ; i++){
        NU32DIP_ReadUART1(message_in, sizeof(message_in));
        
        sscanf(message_in, " %f", &ref_ang);
        int enc_val = Deg2Encoder(ref_ang);
        ref_encoder_traj[i] = enc_val;
      }

      // PrintUART1("last msg in [%s] " , message_in);
      // PrintUART1("i is %d  " , i);
      for ( i=0 ; i<ref_traj_size ; i++){
        PrintUART1("%d:", ref_encoder_traj[i]);
      }
      PrintUART1("Traj got %d numbers, Traj start with encoder %d, end %d\r\n", ref_traj_size,
                 ref_encoder_traj[0], ref_encoder_traj[ref_traj_size - 1]);
      NU32DIP_GREEN = 0;
      NU32DIP_YELLOW = 1;
      break;
    }

    case 'o': {
      NU32DIP_GREEN = 1;
      NU32DIP_YELLOW = 0;
      set_state(s_TRACK);
      while (get_state() == s_TRACK) {
        // wait for it
      }

      PrintUART1("%d\r\n", ref_traj_size);
      for (int i = 0; i < ref_traj_size; i++) {
        // send plot data to MATLAB
        // when first number sent = 1, MATLAB knows we’re done
        double measure_ang = Encoder2Deg(measure_enc_traj[i]);
        sprintf(message, "%f \r\n", measure_ang);
        NU32DIP_WriteUART1(message);
      }

      NU32DIP_GREEN = 0;
      NU32DIP_YELLOW = 1;
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
      PrintUART1("msin [%s]\n" , message_in);
      break;
    }
    }
  }
  return 0;
}


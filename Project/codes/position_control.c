#include "position_control.h"
#include "current_control.h"
#include "ina219.h"
#include "nu32dip.h"
#include "utilities.h"
#include <math.h>
#include <stdbool.h>
#include <xc.h>

volatile float position_p_g = 70.0;
volatile float position_i_g = 0.0;
volatile float position_d_g = 60;
volatile int target_encoder_g;

volatile int ref_traj_size = 0;
int ref_encoder_traj[TrajMax] ;
int measure_enc_traj[TrajMax] ;

static volatile int pos_error_integral = 0;
static volatile int prev_pos_error = 0;

void PosCtlSetup() {
  // Timer4 with ISR

  // T4 – Timer4 | IRQ 19 | VECtor 16 | IFS0<19> IEC0<19> | IPC4<4:2> | IPC4<1:0> | No

  T4CONbits.TCS = 0;       // PBCLK
  T4CONbits.TCKPS = 0b010; // Prescaler 1:4

  // 48M / 200Hz = 240000
  //  240000 / 4 (prescaler) = 60000
  PR4 = 60000;
  TMR4 = 0; // clear value

  T4CONbits.TGATE = 0; // not gated input (the default)

  IPC4bits.T4IP = 2; // Priority 2
  IPC4bits.T4IS = 0;
  IFS0bits.T4IF = 0; // clear interrupt flag
  IEC0bits.T4IE = 1; // enable timer interrupt

  T4CONbits.ON = 1;
}

float PosPID(int target_encoder, int current_encoder) {
  int error = target_encoder - current_encoder;

  pos_error_integral += error;
  float error_dot = ((float)(prev_pos_error - error)); // we just use 0.005s as deritive unit
  float cmd_p = position_p_g * error;
  while (abs(cmd_p) > 2000){
    cmd_p = cmd_p/2;
  }
  float command = cmd_p + position_i_g * pos_error_integral
     + position_d_g * error_dot;

  prev_pos_error = error;
  return command;
}

void __ISR(_TIMER_4_VECTOR, IPL2SOFT) PosController(void) {
  // NU32DIP_GREEN ^=1;
  // NU32DIP_YELLOW ^=1;
  static int track_index = 0;
  switch (get_state()) {
  case s_IDLE: {
    // clear current command to zero in idle state
    pos_ctl_current_ma_g = 0;
    pos_error_integral = 0;
    prev_pos_error = 0;
    track_index =0;
    break;
  }
  case s_HOLD: {
    NU32DIP_GREEN ^= 1;
    // DO a set point PID loop
    float new_target_current = PosPID(target_encoder_g, ReadEncoder());

    if (new_target_current >0 ){
        NU32DIP_YELLOW =0;
    } else{
        NU32DIP_YELLOW =1;
    }
    pos_ctl_current_ma_g = new_target_current;
    break;
  }
    case s_TRACK: {
    
    int current_enc = ReadEncoder();

    float new_target_current = PosPID(ref_encoder_traj[track_index], current_enc);
    measure_enc_traj[track_index] = current_enc;
    if (new_target_current > 0) {
      NU32DIP_YELLOW = 0;
    } else {
      NU32DIP_YELLOW = 1;
    }
    pos_ctl_current_ma_g = new_target_current;
    if (track_index > ref_traj_size) {
      set_state(s_HOLD);
      track_index = 0;
    }
    track_index++;
    }
    }

  IFS0bits.T4IF = 0; // clear interrupt flag
}
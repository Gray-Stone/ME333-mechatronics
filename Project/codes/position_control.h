#ifndef MECH333_POSITION_CONTROL_H
#define MECH333_POSITION_CONTROL_H

extern volatile float position_p_g;
extern volatile float position_i_g;
extern volatile float position_d_g;
extern volatile int target_encoder_g;


void PosCtlSetup();

#endif
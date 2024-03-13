#ifndef MECH333_POSITION_CONTROL_H
#define MECH333_POSITION_CONTROL_H

#define TrajMax 1000


extern volatile float position_p_g;
extern volatile float position_i_g;
extern volatile float position_d_g;
extern volatile int target_encoder_g;

extern volatile int ref_traj_size;
extern int ref_encoder_traj[TrajMax];
extern int measure_enc_traj[TrajMax];

void PosCtlSetup();

#endif
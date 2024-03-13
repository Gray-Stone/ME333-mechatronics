#ifndef MECH333_UTILITIES_H
#define MECH333_UTILITIES_H

typedef enum { 
    s_IDLE = 0,
    s_PWM = 1,
    s_ITEST = 2,
    s_HOLD = 3,
    s_TRACK = 4
} PicStats;

PicStats get_state();
void set_state(PicStats new_state);


int ReadEncoder();

int Deg2Encoder (float ang);

float Encoder2Deg(int count);

#endif

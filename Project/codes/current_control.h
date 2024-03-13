#ifndef ME333_CURRENT_CONTROL_H
#define ME333_CURRENT_CONTROL_H


#define MOTOR_DIR PORTBbits.RB6 
#define PWM_REG OC1RS

extern volatile int pwm_value_g;
extern volatile float pos_ctl_current_ma_g;

#define ITEST_PLOTPTS 100    // number of data points to plot
extern volatile float current_pgain_g;
extern volatile float current_igain_g;
extern volatile int IMSRarray[ITEST_PLOTPTS];
extern volatile int IREFarray[ITEST_PLOTPTS];



void HBridgeSetup();
void Timer2Setup5khz();

#endif
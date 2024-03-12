#ifndef ME333_CURRENT_CONTROL_H
#define ME333_CURRENT_CONTROL_H

#define MOTOR_DIR PORTBbits.RB6 
#define PWM_REG OC1RS

extern int pwm_value_g ;
void HBridgeSetup();
void Timer2Setup5khz();

#endif
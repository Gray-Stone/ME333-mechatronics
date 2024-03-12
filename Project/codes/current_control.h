#ifndef ME333_CURRENT_CONTROL_H
#define ME333_CURRENT_CONTROL_H

#define MOTOR_DIR PORTBbits.RB6 

void HBridgeSetup();
void Timer2Setup5khz();

#endif
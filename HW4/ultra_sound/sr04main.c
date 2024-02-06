#include "nu32dip.h" 


#include "sr04.h"


#define MAX_MESSAGE_LENGTH 100



int main (){
    
    SR04_Startup();
    NU32DIP_Startup();


    char message[MAX_MESSAGE_LENGTH];
    sprintf(message , "Starting range finding");
    NU32DIP_WriteUART1(message);

    while(1){
        float dis = SR04_read_meters();
        sprintf(message, "Distance: %f\n\r", dis);
        // unsigned int dis = SR04_read_raw(24000000);
        // sprintf(message, "Distance: %d\n\r", dis);
        NU32DIP_WriteUART1(message);
        NU32DIP_GREEN = !NU32DIP_GREEN;
        NU32DIP_YELLOW = !NU32DIP_YELLOW;

        unsigned int start_time = _CP0_GET_COUNT();
        // 0.2 sec wait
        while (_CP0_GET_COUNT() - start_time < 2400000){
            // busy waiting at 48Mhz /2
        }
    }

    return 0;

}
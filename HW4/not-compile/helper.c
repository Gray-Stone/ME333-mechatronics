

#include <stdio.h> // input/output library
#include "helper.h"
#include "nu32dip.h"


#define MAX_INPUT_LENGTH 100

void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years;
       i = i + 1) { 
    invp->invarray[i] = invp->growth * invp->invarray[i - 1];
  }
} // ***** END calculateGrowth *****

int getUserInput(Investment *invp) {

  int valid; 

  char out_string[80];
  char user_input[MAX_INPUT_LENGTH];

  // Change to using sprintf and write uart
  sprintf(
      out_string,
      "Enter investment, growth rate, number of yrs (up to %d): ", MAX_YEARS);
  NU32DIP_WriteUART1(out_string);

  // Change to using sscanf and read uart
  NU32DIP_ReadUART1(user_input, MAX_INPUT_LENGTH);
  sscanf(user_input, "%lf %lf %d", &(invp->inv0), &(invp->growth),
         &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) && (invp->years > 0) &&
          (invp->years <= MAX_YEARS);

  // Change to using sprintf and write uart
  sprintf(out_string, "Valid input?  %d\n\r", valid);
  NU32DIP_WriteUART1(out_string);

  if (!valid) { 
    // Change to using sprintf and write uart
    sprintf(out_string, "Invalid input; exiting.\n\r");
    NU32DIP_WriteUART1(out_string);
  }
  return (valid);
} // ***** END getUserInput *****

void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100]; 

  char out_string[80];

  // Change to using sprintf and write uart
  sprintf(out_string, "\n\rRESULTS:\n\r\n\r");
  NU32DIP_WriteUART1(out_string);

  for (i = 0; i <= yrs; i++) { 
  // Change to using sprintf and write uart
    sprintf(out_string, "Year %3d:  %10.2f\n\r", i, arr[i]);
    NU32DIP_WriteUART1(out_string);
  }

  // Change to using sprintf and write uart
  sprintf(out_string, "\n\r\n\r");
  NU32DIP_WriteUART1(out_string);

} // ***** END sendOutput *****

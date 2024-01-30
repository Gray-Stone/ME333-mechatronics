#include <stdio.h> // input/output library

#include "nu32dip.h"
#define MAX_YEARS 100 // constant indicating max number of years to track
#define MAX_INPUT_LENGTH 100
typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS + 1]; // investment array   ==SecA.4.9==
} Investment;                     // the new data type is called Investment

int getUserInput(Investment *invp);     // invp is a pointer to type ...
void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==
void sendOutput(double *arr, int years);

int main(void) {

  Investment inv; // variable definition, ==SecA.4.5==

  while (getUserInput(&inv)) {  // while loop ==SecA.4.13==
    inv.invarray[0] = inv.inv0; // struct access ==SecA.4.4==
    calculateGrowth(&inv);      // & referencing (pointers) ==SecA.4.6, A.4.8==
    sendOutput(inv.invarray,    // passing a pointer to an array ==SecA.4.9==
               inv.years);      // passing a value, not a pointer ==SecA.4.6==
  }
  return 0; // return value of main ==SecA.4.6==
} // ***** END main *****

void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years;
       i = i + 1) { // relational operators ==SecA.4.10==
                    // struct access ==SecA.4.4==
    invp->invarray[i] = invp->growth * invp->invarray[i - 1];
  }
} // ***** END calculateGrowth *****

int getUserInput(Investment *invp) {

  int valid; // int used as a boolean ==SecA.4.10==

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

  if (!valid) { // ! is logical NOT ==SecA.4.11==
    // Change to using sprintf and write uart
    sprintf(out_string, "Invalid input; exiting.\n\r");
    NU32DIP_WriteUART1(out_string);
  }
  return (valid);
} // ***** END getUserInput *****

void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100]; // defining a string ==SecA.4.9==

  char out_string[80];

  // Change to using sprintf and write uart
  sprintf(out_string, "\n\rRESULTS:\n\r\n\r");
  NU32DIP_WriteUART1(out_string);

  for (i = 0; i <= yrs; i++) { // ++, +=, math in ==SecA.4.7==
  // Change to using sprintf and write uart
    sprintf(out_string, "Year %3d:  %10.2f\n\r", i, arr[i]);
    NU32DIP_WriteUART1(out_string);
  }

  // Change to using sprintf and write uart
  sprintf(out_string, "\n\r\n\r");
  NU32DIP_WriteUART1(out_string);

} // ***** END sendOutput *****

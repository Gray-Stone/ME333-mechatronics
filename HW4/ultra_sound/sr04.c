#include "sr04.h"
#include <xc.h>

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// initialize the pins used by the SR04
void SR04_Startup() {
  ANSELA = 0; // turn off the analog input functionality that overrides
              // everything else
  ANSELB = 0;
  TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
  TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
  TRIG = 0;              // initialize TRIG to LOW
}

// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(unsigned int timeout) {
  while (ECHO == 1) {
    // Make sure sensor is ready to be used
  }
  // turn on TRIG
  TRIG = 1;
  // wait at least 10us
  unsigned int start_time = _CP0_GET_COUNT();

  // 1/ (24 000 000) * 240

  while (_CP0_GET_COUNT() - start_time < 240) {
    // busy waiting, 48Mhz /2
  }
  // turn off TRIG
  TRIG = 0;
  // wait until ECHO is on

  while (ECHO == 0) {
    // busy waiting
  }
  // note the value of the core timer
  start_time = _CP0_GET_COUNT();

  // wait until ECHO is off or timeout core ticks has passed
  while (ECHO != 0) {
    if (_CP0_GET_COUNT() - start_time > timeout) {
      return -1;
      break;
    }
    // busy waiting
  }
  unsigned int end_time = _CP0_GET_COUNT();
  // note the core timer
  return end_time - start_time;
  // return _CP0_GET_COUNT() - start_time;
  // return the difference in core times
}

float SR04_read_meters() {
  // read the raw rs04 value
  unsigned int time =
      SR04_read_raw(24 * 3000); // roughly the amount of 1 meter.

  float distance = ((float)time) / 24000000.0 * 346 / 2;
  return distance;

  // convert the time to meters, the velocity of sound in air is 346 m/s
  // return the distance in meters
}

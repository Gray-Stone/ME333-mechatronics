#include <xc.h> // Load the proper header for the processor

#define MAXCYCLES 1000000
#define DELTACYCLES 100000

void delay(int cycles);

int main(void) {
  TRISBCLR = 0x30;    // Make TRIS for B4 and B5 0,
  LATBbits.LATB4 = 0; // Turn GREEN on and YELLOW off.  These pins sink current
  LATBbits.LATB5 =
      1; // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"

  int current_delay = MAXCYCLES;

  while (1) {
    delay(current_delay);
    LATBINV = 0b110000; // toggle GREEN and YELLOW;
    // Decrement the cycle count
    if (current_delay < DELTACYCLES) {
      // Check before subtracting, this prevents under flow if using unsigned
      current_delay = MAXCYCLES;
    } else {
      current_delay -= DELTACYCLES;
    }
  }
  return 0;
}

void delay(int cycles) {
  int j;
  for (j = 0; j < cycles; j++) { // number is 1 million
    while (!PORTAbits.RA4) {
      ; // Pin A4 is the USER switch, low (FALSE) if pressed.
    }
  }
}

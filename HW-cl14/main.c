
#include "nu32dip.h"

#define MAX_MESSAGE_LENGTH 100
char message[MAX_MESSAGE_LENGTH];

#define NUMSAMPS 1000
#define SAMPLE_TIME 6 // 24MHz*250ns

// number of points in waveform
static volatile int Waveform[NUMSAMPS];

#define PLOTPTS 200   // number of data points to plot
#define DECIMATION 10 // plot every 10th point

static volatile int ADCarray[PLOTPTS];
static volatile int REFarray[PLOTPTS];
static volatile int StoringData = 0;
static volatile float Kp = 0, Ki = 0;
// waveform
// measured values to plot
// reference values to plot
// if this flag = 1, currently storing
// plot data
// control gains

void makeWaveform() {
  int i = 0, center = 500, A = 300; // square wave, fill in center value and
  // amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if (i < NUMSAMPS / 2) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

void Timer2_OC1_Setup() {
  // The LED is connected to PB15, Thus we set output PPS to this pin
  RPB15R = 0b0101; // Setup OC1 to output to PB15

  PR3 = 2399;            // set period to 48M / 2400 = 20k
  TMR3 = 0;              // clear count
  T3CONbits.TCS = 0;     // use PBCLK
  T3CONbits.TCKPS = 0;   // prescale of 1
  OC1CONbits.OCFLT = 0;  // Turn off fault
  OC1CONbits.OCTSEL = 1; // Set clock source to timer 3
  OC1CONbits.OCM =
      0b110;    // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1800; // comp 1 set to 24000 * 0.75 = 18000
  T3CONbits.ON = 1;  // Turn on timer 3
  OC1CONbits.ON = 1; // turn on OC1
}

void Timer2Setup() {
  T2CONbits.TCS = 0;      // use PBCLK
  T2CONbits.TCKPS = 0b00; // set prescaler to 0
  // 48MHz / 1k = 48K
  PR2 = 48000;         // Select limit.
  TMR2 = 0;            // Clear value.
  T2CONbits.TGATE = 0; // not gated input (the default)
  T2CONbits.TCS = 0;   // PCBLK input (the default)

  // T2 – Timer2 9  8  IFS0<9>  IEC0<9>  IPC2<4:2>  IPC2<1:0>
  IPC2bits.T2IP = 5; // priority 5
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0; // clear interrupt flag
  IEC0bits.T2IE = 1; // enable timer interrupt.

  T2CONbits.ON = 1; // timer on
}

void ADC0Setup() {
  // ADC setup
  // Using Pin2 for phototrans input, AN0.
  // VREF+/CVREF+/AN0/C3INC/RPA0/CTED1/RA0
  TRISASET = 0b1; // Set A0 as input
  ANSELASET =
      0b1; // This should be the default, but still setting it just in case
  ANSELAbits.ANSA0 = 1; // AN0 set to analog
  AD1CON3bits.ADCS = 1; // ADC clock period is Tad = 2*(ADCS+1)*Tpb
                        // =2*2*(1/48000000Hz) = 83ns > 75ns
  AD1CON1bits.ADON = 1; // turn it on
}

unsigned int adc_sample_convert(int pin) {
  unsigned int elapsed = 0, finish_time = 0;
  AD1CHSbits.CH0SA = pin;
  AD1CON1bits.SAMP = 1; // starts sampling
  elapsed = _CP0_GET_COUNT();
  finish_time = elapsed + SAMPLE_TIME;
  while (_CP0_GET_COUNT() < finish_time) {
    ;
  }
  AD1CON1bits.SAMP = 0;
  while (!AD1CON1bits.DONE) {
    ;
  }
  return ADC1BUF0;
}

int err_int = 0; // Intergal error

// We cannot use shadow register since our chip doesn't have it
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
  //   Toggle the LED pin for a debugging signal.
  NU32DIP_GREEN = !NU32DIP_GREEN;

  static int counter = 0; // initialize counter once
  static int plotind = 0; // index for data arrays; counts up to PLOTPTS
  static int decctr = 0;  // counts to store data one every DECIMATION
  static int adcval = 0;

  // initialize counter once
  // Change duty cycle by setting OC1RS
  int desire = Waveform[counter];

  // Read the ADC value.
  adcval = adc_sample_convert(0);

  int error = desire-adcval; 
  err_int = err_int + error;

  float u = Kp*error + Ki*err_int;

  double unew = u + 50.0;
  if (unew > 100.0) {
  unew = 100.0;
  } else if (unew < 0.0) {
  unew = 0.0;
  }
    
  OC1RS = (unsigned int) ((unew/100.0) * PR3);

  if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) {
      // after DECIMATION control loops,
      // reset decimation counter
      decctr = 0;
      // store data in global arrays
      ADCarray[plotind] = adcval;
      REFarray[plotind] = Waveform[counter];
      // increment plot data index
      plotind++;
    }
    if (plotind == PLOTPTS) {
      // if max number of plot points plot is reached,
      // reset the plot index
      plotind = 0;
      // tell main data is ready to be sent to MATLAB
      StoringData = 0;
    }
  }

  counter++;
  // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0;
    // roll the counter over when needed
  }
  IFS0bits.T2IF = 0;
}

int main() {

  NU32DIP_Startup();
  makeWaveform();
  // Disable interrupt during setup
  __builtin_disable_interrupts();
  Timer2_OC1_Setup();
  Timer2Setup();

  ADC0Setup();

  __builtin_enable_interrupts();

  sprintf(message, "Value of DEVCFG1  %x \n\r", DEVCFG1);

  NU32DIP_WriteUART1(message);

  float kptemp = 0, kitemp = 0;
  // temporary local gains
  int i = 0;
  // plot data loop counter

  while (1) {
    NU32DIP_ReadUART1(message, sizeof(message));
    // wait for a message from MATLAB
    sscanf(message, "%f %f", &kptemp, &kitemp);

    // keep ISR disabled as briefly as possible
    __builtin_disable_interrupts();
    // copy local variables to globals used by ISR
    Kp = kptemp;
    Ki = kitemp;
    // only 2 simple C commands while ISRs disabled
    __builtin_enable_interrupts();

    // message to ISR to start storing data
    StoringData = 1;
    while (StoringData) {
      // wait until ISR says data storing is done
      ; // do nothing
    }
    for (i = 0; i < PLOTPTS; i++) {
      // send plot data to MATLAB
      // when first number sent = 1, MATLAB knows we’re done
      sprintf(message, "%d %d %d\r\n", PLOTPTS - i, ADCarray[i], REFarray[i]);
      NU32DIP_WriteUART1(message);
    }
  }

  return 0;
}
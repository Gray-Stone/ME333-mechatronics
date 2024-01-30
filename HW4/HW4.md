 
Chapter 3, problems 3 (for part a, ports A and B, and what pin is B0), 4,5,7,9 (for TRISB instead of TIRSD).

Chapter 4, problems 1, 2

Write a library for the ultrasonic range finder (HC-SR04) using the template sr04.c and sr04.h in the Files section of Canvas. Build the HC-SR04 circuit and test the sensor. Turn in your code and a video demonstrating the PIC printing the distance from the sensor in meters 4 times per second.



## 3 

Refer to the Memory Organization section of the Data Sheet and Figure 2.1.

### a. 
Referring to the Data Sheet, indicate which bits, 0-31, can be used as input/outputs for
each of Ports B through G. For the PIC32MX795F512H in Figure 2.1, indicate which
pin corresponds to bit 0 of port E (this is referred to as RE0).

For ports A and B:

Memory at 0xBF88 6000 are reserved for ports A to C.

### b.
The SFR INTCON refers to “interrupt control.” Which bits, 0-31, of this SFR are
unimplemented? Of the bits that are implemented, give the numbers of the bits and
their names.

For `INTCON` , bit <31:13> <11> <7:5> are unimplemented.

bit 12 is MVEC 
bit <10:8> is TPC
bit <4:0> is INT4EP to INT0EP

## 4 

Modify simplePIC.c so that both lights are on or off at the same time, instead of opposite
each other. Turn in only the code that changed.
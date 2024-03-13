#include "utilities.h"
#include "encoder.h"
#include <math.h>

static PicStats current_state;

PicStats get_state(){
    return current_state;
}

void set_state(PicStats new_state){
    current_state = new_state;
}

int ReadEncoder() {
  WriteUART2("a");
  while (!get_encoder_flag()) {
    // Busy waiting.
  }
  set_encoder_flag(0);

  return get_encoder_count();
}

int Deg2Encoder (float ang){

  return (int) (ang / 360.0 * 384.0);
}

float Encoder2Deg(int count) {
  // Encoder have 96 line.
  // Pico gives 384 count per rotation
  // (96*4) / (360) deg

  return ((float)count) / 384.0 * 360.0;
}
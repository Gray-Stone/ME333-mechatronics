#include "utilities.h"

static PicStats current_state;

PicStats get_state(){
    return current_state;
}

void set_state(PicStats new_state){
    current_state = new_state;
}

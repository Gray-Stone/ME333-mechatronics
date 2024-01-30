
// These include for ch4 p2 b)
// #include "helper.h" 

// These include for ch4 p2 c)
#include "invest_type.h" // might be redundent but safe to include again as this file is expicatlly dependent on invest type
#include "calculate.h"
#include "io.h"

int main(void) {

  Investment inv; 

  while (getUserInput(&inv)) {  
    inv.invarray[0] = inv.inv0; 
    calculateGrowth(&inv);      
    sendOutput(inv.invarray,    
               inv.years);      
  }
  return 0; 
} // ***** END main *****


//change your MCU
#include <16f877.h>
#include "CCSPIC_AD9850.h"
// 4MHz clock
#use delay(clock=4000000)

void main(){

   set_tris_b(W_CLK | FQ_UD | DATA | RESET);
   
   AD9850_Init();
   AD9850_Reset();
   AD9850_Osc(1234, 0);

}

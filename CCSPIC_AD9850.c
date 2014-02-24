//change the mcu if you are using another one.
#include <16f877.h>
#include "CCSPIC_AD9850.h"

/* Starts AD9850 operation changing its value to "all zeros".
 * Refreshes previous status from the microcontroller.
 * */
void AD9850_Init(void){
   output_low(W_CLK | FQ_UD | DATA | RESET);
}
 
/* Reset operation for the AD9850. This function must be called before using AD9850_Osc
 * first time in the execution (check page 12 on datasheet)
 * */
void AD9850_Reset(void){

   output_low(W_CLK);
   output_high(W_CLK);
   output_low(W_CLK);

   output_low(RESET);
   output_high(RESET);
   output_low(RESET);
   
   output_low(FQ_UD);
   output_high(FQ_UD);
   output_low(FQ_UD);

   AD9850_Osc(0, 0);
}

/* Sets the DDS sine and square oscillator to the detailed "frequency" and "phase" variables.
 * "frequency" will be turned into a 32 bit word, so the frequency resolution of 0.0291 Hz
 * with a 125 MHz reference clock. "phase" will be a 5 bit word instead so the resolution is
 * 11.5 degrees, or pi/32 radians.
 */
void AD9850_Osc(double frequency, double phase){
   int i;
   long  y = frequency * FREQ_FACTOR / XTAL_MHZ;

   while(phase > 360)
      phase -= 360;
   long z = phase / 11.5;

   //Frequency 32-bit word
   for (i = 31; i >= 0; i--){
      output_bit(DATA, ((y >> (31 - i)) & 0x01));
      output_high(W_CLK);
      output_low(W_CLK);
   }
   //control bit #1, control bit #2 and Power off, all to low
   output_low(DATA);
   output_high(W_CLK);
   output_low(W_CLK);
   output_high(W_CLK);
   output_low(W_CLK);
   output_high(W_CLK);
   output_low(W_CLK);

   //phase 5-bit word
   for (i = 5; i >= 0; i--){
      output_bit(DATA, ((z >> (5 - i)) & 0x01));
      output_high(W_CLK);
      output_low(W_CLK);
   }

   output_high(FQ_UD);
   output_low(FQ_UD);
}

/* Enables power down mode. This method is used for a quick "all outputs" disable.
 * The effect is the same as AD9850_Osc(0,0), but it takes less clock cycles
 */
void AD9850_PowerDown(void){
   int PDword = 0x04;
   int i;
   output_high(FQ_UD);
   output_low(FQ_UD);

   for (i = 0; i < 8; i++){
      output_bit(DATA,(PDword >> i) & 0x01);
      output_high(W_CLK);
      output_low(W_CLK);
   }

   output_high(FQ_UD);
   output_low(FQ_UD);

}

/* Performs a frequency sweep increased in "inc"Hz steps. The frequency order is from low to high
 * and resets to the lower frequency when maximum frequency is reached.
 */
void AD9850_Sweep_Up(double minFreq, double maxFreq, double inc, int cyclesPerDelay){
   double f = minFreq;
   int i;
   while (1){
      AD9850_Osc(f, 0);
      f += inc;
      if (f > maxFreq)
         f = minFreq;

      delay_cycles (cyclesPerDelay);
   }
}

/* Performs a frequency sweep decreased in "inc"Hz steps. The frequency order is from high to low
 * and resets to the higher frequency when minimum frequency is reached.
 */
void AD9850_Sweep_Down(double minFreq, double maxFreq, double inc, int cyclesPerDelay){
   double f = maxFreq;
   int i;
   while (1){
      AD9850_Osc(f, 0);
      f -= inc;
      if (f < minFreq)
         f = maxFreq;
      delay_cycles (cyclesPerDelay);
   }
}

/* Performs a frequency sweep increased in "inc"Hz steps. The frequency order is from low to high,
 * but it changes to from high to low when the maximum frequency is reached and so on.
 */
void AD9850_Sweep_Loop(double minFreq, double maxFreq, double inc, int cyclesPerDelay){
   double f = minFreq;
   int i;
   while (1){
      AD9850_Osc(f, 0);
      f += inc;
      if (f > maxFreq || f < minFreq)
         inc = -inc;
      delay_cycles (cyclesPerDelay);
   }
}





#include "LPC1768Function.h"
static void loop() {
   int i,j;
  
   for(i=0; i<10000; i++) 
      for (j=0; j<10000; j++); 
}

int main() {
	LEDSetup();
   while(1) {
	LED_On(1);
       loop();
       LED_Off(2);
       loop();
   } 
   return 0;
}

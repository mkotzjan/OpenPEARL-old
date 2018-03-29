#include "PearlIncludes.h"

pearlrt::Lpc17xxClock myClock(0);
pearlrt::Lpc17xxDigitalOut leds(2,7,8);
pearlrt::Lpc17xxDigitalIn joy(1,29,5);

SPCTASK(TASK1);

DCLTASK(TASK1, pearlrt::Prio(110), pearlrt::BitString<1>(1)) {

   pearlrt::BitString<8> bits(1);
   pearlrt::BitString<5> jbits;
   pearlrt::Fixed<15> eins(1);

   pearlrt::Log::info("FreeRTOS Task-1 is running");
   leds.dationOpen(NULL,pearlrt::Dation::OUT);
   joy.dationOpen(NULL,pearlrt::Dation::IN);

   while(1) {
      pearlrt::Log::info("send %x to leds", bits.x);
      leds.dationWrite(&bits, sizeof(bits));
      bits = bits.bitCshift(eins);
      joy.dationRead(&jbits, sizeof(jbits));
      printf("joystick: %x\n", jbits.x);
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(1.0));
   }
}


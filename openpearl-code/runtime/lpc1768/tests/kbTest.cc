#include "PearlIncludes.h"

pearlrt::Lpc17xxClock myClock(0);
pearlrt::Lpc17xxUsbKeyboard kb;
//pearlrt::Lpc17xxUart kb(0,115200,8,1,"N",false);

SPCTASK(TASK1);

DCLTASK(TASK1, pearlrt::Prio(110), pearlrt::BitString<1>(1)) {

   char ch;

   pearlrt::Log::info("Task1 is running");
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(), pearlrt::Duration(20.0));
   pearlrt::Log::info("open keyboard");
   
   kb.dationOpen(NULL,0);

   while(1) {
      kb.dationRead(&ch, 1);
      printf("char = %x\n", ch);
   }
}


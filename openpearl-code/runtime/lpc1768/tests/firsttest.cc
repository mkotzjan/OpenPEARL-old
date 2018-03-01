#define _POSIX_TIMERS
#include "time.h"
//#include "PearlIncludes.h"
#include "pearl.h"

extern "C" {
#include "hy32a.h"
}


SPCTASK(TASK1);
SPCTASK(TASK2);
SPCTASK(TASK3);

void printTimeAndDay(int line) {
      struct timespec ts;
      char buf[26];
      clock_gettime(0,&ts);
      ts.tv_sec+=2*60*60;//Mitteleuropaeische Sommerzeit
      ctime_r(&ts.tv_sec, buf);
      GUI_Text(1,line,(uint8_t*)buf,Green,Black);
}

DCLTASK(TASK1, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {
   pearlrt::Log::info("FreeRTOS Task-1 is running");
   LCD_Initializtion();
   LCD_Clear(Black);

   TASK2.activate(me);

   TASK3.activate(me,pearlrt::Task::AFTER |pearlrt::Task::ALL,
           pearlrt::Prio(), pearlrt::Clock(),
              pearlrt::Duration(2.0),   // after
              pearlrt::Duration(3.0));  // all

   for (;;) {

      GUI_Text(100,300,(uint8_t*)"TASK1: Tick",Blue,White);
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.2));
      GUI_Text(100,300,(uint8_t*)"TASK1: Tack",White,Blue);
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.4));
   }
}


DCLTASK(TASK2, pearlrt::Prio(2), pearlrt::BitString<1>(0)) {
   pearlrt::Log::info("FreeRTOS Task-2 running!");

   for (;;) {
	   printTimeAndDay(0);
      GUI_Text(100,200,(uint8_t*)"TASK2: Tick",Blue,White);
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.6));
      GUI_Text(100,200,(uint8_t*)"TASK2: Tack",Blue2,White);
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.3));
   }
}


DCLTASK(TASK3, pearlrt::Prio(3), pearlrt::BitString<1>(0)) {
      pearlrt::Log::info("FreeRTOS Task-3 is running!");
}

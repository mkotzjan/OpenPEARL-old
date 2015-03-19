#include "PearlIncludes.h"

extern "C" {
#include "hy32a.h"
}


SPCTASK(TASK1);
SPCTASK(TASK2);
SPCTASK(TASK3);

void printTimeAndDay(int line) {
      RTC_TIME_T tm;
      char buf[25];
      Chip_RTC_GetFullTime(LPC_RTC, &tm);
      sprintf(buf,"%02d.%02d.%04d %02d:%02d:%02d",
           (int)tm.time[RTC_TIMETYPE_DAYOFMONTH],
           (int)tm.time[RTC_TIMETYPE_MONTH],
           (int)tm.time[RTC_TIMETYPE_YEAR], 
           (int)tm.time[RTC_TIMETYPE_HOUR],
	   (int)tm.time[RTC_TIMETYPE_MINUTE],
           (int)tm.time[RTC_TIMETYPE_SECOND]);
      GUI_Text(1,line,(uint8_t*)buf,Black,White);
      if (line < 180)
         GUI_Text(1,line+20,(uint8_t*)"                   ",Black,White);
      else
         GUI_Text(1,20,(uint8_t*)"                   ",Black,White);
}

DCLTASK(TASK1, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {
   pearlrt::Log::info("FreeRTOS Task-1 is running");
   LCD_Initializtion();
   LCD_Clear(Blue);

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
//      LED_On(7);
      GUI_Text(100,300,(uint8_t*)"TASK1: Tack",White,Blue);
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.4));
//      vTaskDelay(1000);
//      LED_Off(7);
   }
}


DCLTASK(TASK2, pearlrt::Prio(2), pearlrt::BitString<1>(0)) {
   pearlrt::Log::info("FreeRTOS Task-2 running!");
   int line=20;

   for (;;) {
	   printTimeAndDay(line);
	   line += 20;
	   if (line > 180) line = 20;
      GUI_Text(100,200,(uint8_t*)"TASK2: Tick",Blue,White);
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.6));
//      vTaskDelay(600);
//      LED_Off(5);
      GUI_Text(100,200,(uint8_t*)"TASK2: Tack",Blue2,White);
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.3));
//      vTaskDelay(200);
//      LED_On(5);
   }
}


DCLTASK(TASK3, pearlrt::Prio(3), pearlrt::BitString<1>(0)) {
      pearlrt::Log::info("FreeRTOS Task-3 is running!");
}



#include "PearlIncludes.h"

pearlrt::Lpc17xxClock myClock(0);


SPCTASK(TASK1);
SPCTASK(TASK2);
SPCTASK(TASK3);
SPCTASK(TASK4);
SPCTASK(pokeCanary);

void printTimeAndDay(int line) {
//      struct timespec ts;
      char buf[26];
//      clock_gettime(0,&ts);
//      ts.tv_sec+=2*60*60;//Mitteleuropaeische Sommerzeit
//      ctime_r(&ts.tv_sec, buf);
//      vTaskPrioritySet(NULL,289);
//      printf(buf);
}

static void Canary(void *pcParameters){

	while(true){
	printf("Canary: RTOSCanary @1\n");
	vTaskDelay(1100);
	printf("Canary: RTOSCanary @2\n");
	vTaskDelay(1100);
		}
}

DCLTASK(pokeCanary, pearlrt::Prio(100), pearlrt::BitString<1>(0)) {
	static int i=0;
	if(i==0){
	printf("            Canary: pokeCanary @1\n");
		i=1;
		printf("            piep\n");
	}
	else{
	printf("            Canary: pokeCanary @2\n");
		i=0;
	}

}

DCLTASK(TASK1, pearlrt::Prio(110), pearlrt::BitString<1>(1)) {


   pearlrt::Log::info("FreeRTOS Task-1 is running");

   pokeCanary.activate(me, pearlrt::Task::ALL,
           pearlrt::Prio(), pearlrt::Clock(),
              pearlrt::Duration(20.3333),   // after
              pearlrt::Duration(2.0));  // all


   TASK2.activate(me);

   TASK3.activate(me,pearlrt::Task::AFTER |pearlrt::Task::ALL,
           pearlrt::Prio(), pearlrt::Clock(),
              pearlrt::Duration(20.3333),   // after
              pearlrt::Duration(1.0));  // all

/*
   TASK4.activate(me,pearlrt::Task::AFTER |pearlrt::Task::ALL,
              pearlrt::Prio(), pearlrt::Clock(),
                 pearlrt::Duration(0.50),   // after
                 pearlrt::Duration(0.05));  // all

*/
   for (;;) {

      printf("Task1:8Hz @1\n");
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(1.25));
      printf("Task1:8Hz @2\n");
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(1.25));
   }
}


DCLTASK(TASK2, pearlrt::Prio(120), pearlrt::BitString<1>(0)) {
   pearlrt::Log::info("FreeRTOS Task-2 running!");

   for (;;) {
      printf("      Task2:4Hz @1\n");
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.25));
      printf("      Task2:4Hz @2\n");
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.25));
   }
}


DCLTASK(TASK3, pearlrt::Prio(50), pearlrt::BitString<1>(0)) {
      printTimeAndDay(0);
}

DCLTASK(TASK4, pearlrt::Prio(11), pearlrt::BitString<1>(0)) {
	static unsigned int color=0;
	color&=0xffff;
      printf("            Task4:20Hz \n");
	color+=0x0731;
}

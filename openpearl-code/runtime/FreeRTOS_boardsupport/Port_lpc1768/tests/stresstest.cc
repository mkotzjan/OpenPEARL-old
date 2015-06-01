#define _POSIX_TIMERS
#include "time.h"
#include "PearlIncludes.h"

extern "C" {
void timer_table_check_dbg();
#include "hy32a.h"
#include "FreeRTOS.h"
#include "task.h"
struct sigevent{};
	timer_t mytimer;
	struct { void (*cb)(void *arg_ptr);
		void *th;
		}callback;
		static struct tablestate_t{
				unsigned full:1;
				unsigned active:1;
				unsigned unsorted:1;
				unsigned tableinitialized:1;
				unsigned sorttaskreset:1;
			}ts;
}

TaskHandle_t CanaryHandle;
SPCTASK(TASK1);
SPCTASK(TASK2);
SPCTASK(TASK3);
SPCTASK(TASK4);

void printTimeAndDay(int line) {
      struct timespec ts;
      char buf[26];
      clock_gettime(0,&ts);
      ts.tv_sec+=2*60*60;//Mitteleuropaeische Sommerzeit
      ctime_r(&ts.tv_sec, buf);
      vTaskPrioritySet(NULL,289);
      GUI_Text(1,line,(uint8_t*)buf,Green,Black);
}

static void Canary(void *pcParameters){

	while(true){
	GUI_Text(50,220,(uint8_t*)"RTOSCanary",Black,Yellow);
	vTaskDelay(1100);
	GUI_Text(50,220,(uint8_t*)"RTOSCanary",Yellow,Black);
	vTaskDelay(1100);
		}
}

static void pokeCanary(void *su){
	static int i=0;
	if(i==0){
		GUI_Text(50,120,(uint8_t*)"TCanary",Black,Yellow);
		i=1;
		printf("piep\n");
	}
	else{
		GUI_Text(50,120,(uint8_t*)"TCanary",Black,Green);
		i=0;
	}

}

DCLTASK(TASK1, pearlrt::Prio(110), pearlrt::BitString<1>(1)) {

	timer_create(0,(struct sigevent*)&callback,&mytimer);

	callback.cb=pokeCanary;
	callback.th=NULL;

   pearlrt::Log::info("FreeRTOS Task-1 is running");
   LCD_Initializtion();
   LCD_Clear(Black);
   xTaskCreate(Canary,"Canary",100,NULL,250,&CanaryHandle);
   struct itimerspec myitmspec={0};
   myitmspec.it_interval.tv_sec=2;
   timer_settime(mytimer,0,&myitmspec,NULL);


   TASK2.activate(me);

   TASK3.activate(me,pearlrt::Task::AFTER |pearlrt::Task::ALL,
           pearlrt::Prio(), pearlrt::Clock(),
              pearlrt::Duration(20.3333),   // after
              pearlrt::Duration(1.0));  // all

   TASK4.activate(me,pearlrt::Task::AFTER |pearlrt::Task::ALL,
              pearlrt::Prio(), pearlrt::Clock(),
                 pearlrt::Duration(0.50),   // after
                 pearlrt::Duration(0.05));  // all

   for (;;) {

      GUI_Text(100,300,(uint8_t*)"Task1:8Hz",Blue,White);
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.125));
      GUI_Text(100,300,(uint8_t*)"Task1:8Hz",White,Blue);
      TASK1.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.125));
   }
}


DCLTASK(TASK2, pearlrt::Prio(120), pearlrt::BitString<1>(0)) {
   pearlrt::Log::info("FreeRTOS Task-2 running!");

   for (;;) {
	  GUI_Text(100,200,(uint8_t*)"Task2:4Hz",Blue,White);
      TASK2.resume(Task::AFTER,
                   pearlrt::Clock(),
                   pearlrt::Duration(0.25));
      GUI_Text(100,200,(uint8_t*)"Task2:4Hz",White,Blue);
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
	GUI_Text(100,50,(uint8_t*)"Task4:20Hz",((~color)&0xffff),color);
	color+=0x0731;
}

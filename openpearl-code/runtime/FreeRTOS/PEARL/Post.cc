#define _POSIX_TIMERS
#include <chip.h>
#include <rtc_17xx_40xx.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Post.h"
#include "lpc17_rtc.h"

// labels from the linker script
extern "C" {
// lables defined as char to get the difference in bytes instead of ints
   extern unsigned char __etext;
   extern unsigned char __data_start__, __data_end__;
   extern unsigned char __bss_start__, __bss_end__;
};

void Post::print(void) {
   int bytes;
   struct timespec ts;
   struct tm tm;

   printf("------------\n");
   printf("POST summary\n");
   printf("main clock rate: %lu MHz\n",
          Chip_Clock_GetSystemClockRate() / 1000000);
   printf("Flash usage:\n");
   printf("            code: %p\n", &__etext);
   printf("      data(copy): 0x%x \n", &__data_end__ - &__data_start__);
   bytes = (int)(&__etext) + (__data_end__ - __data_start__);
   printf("           total: %d (%2d %%)\n", 
             bytes, bytes * 100 / (512 * 1024));

   printf("RAM usage:\n");
   printf("           data: %p - %p \n", &__data_start__, &__data_end__);
   printf("           bss : %p - %p\n", &__bss_start__, &__bss_end__);
   bytes = (&__data_end__ - &__data_start__) +
           (&__bss_end__ - &__bss_start__);
   printf("          total: %d (%2d %%)\n", bytes, bytes * 100 / (32 * 1024));

   clock_gettime(CLOCK_REALTIME,&ts);
   tm = *(gmtime(&ts.tv_sec));

   /*
   printf("%d %02d.%02d.%04d %02d:%02d:%02d\n ", 
          tm.tm_wday, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900,
          tm.tm_hour, tm.tm_min, tm.tm_sec);
   */
   printf(ctime(&ts.tv_sec));
   printf("------------\n");
}

void Post::config(void) {
   struct tm tm;
   char line[80];

   while (1) {
      printf("commands: \n");
      printf("RTC: DOW YYYY:MM:DD:HH:MIN:SEC  set full date (DOW:1=Mon)\n");

      fgets(line, sizeof(line) - 1, stdin);

      if (7 == sscanf(line, "RTC: %d %d:%d:%d:%d:%d:%d",
                &tm.tm_wday,
                &tm.tm_year,&tm.tm_mon,&tm.tm_mday,
		&tm.tm_hour,&tm.tm_min,&tm.tm_sec)) {
         tm.tm_mon--;
         tm.tm_year-=1900;
         printf(" set RTC to: ");
         printf(asctime(&tm));
         rtc_settime(&tm);
      } else if (strncmp(line, "RUN", 3) == 0) {
         printf("exit POST ... run application\n");
         return;
      } else {
         printf("???? unknown command\n");
      }
   }
}

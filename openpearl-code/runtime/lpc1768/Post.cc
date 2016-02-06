#define _POSIX_TIMERS
#include <chip.h>
#include <rtc_17xx_40xx.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Post.h"
#include "Lpc17xxRTC.h"

// labels from the linker script
extern "C" {
// labels defined as char to get the difference in bytes instead of ints
   extern unsigned char __etext;
   extern unsigned char __data_start__, __data_end__;
   extern unsigned char __bss_start__, __bss_end__;
   extern unsigned char __bss2_start__, __bss2_end__;
};
namespace pearlrt {
   int Post::postStatus = -1;   // preset - no post required

   int Post::getPostStatus() {
      return postStatus;
   }

   Post::Post(int x) {
      if (x < 0 || x > 1) {
         printf("illegal value in Post-pseudo device -- Post() ignored\n");
      } else {
         postStatus = x;
      }
   }
   void Post::print(void) {
      static const struct tm  defaultDate = {0, 0, 0,
                1, 0, 2016 - 1900,
                5, 0, 0
      }; // 1.1.2016 0:00:00

      int bytes;
      struct tm tm;
      uint64_t ns;
      time_t sec;

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
      bytes = (&__data_end__ - &__data_start__) +
              (&__bss_end__ - &__bss_start__);
      printf("      RAM1 : %d (%2d %%)\n", bytes, bytes * 100 / (32 * 1024));
      printf("           data: %p - %p \n", &__data_start__, &__data_end__);
      printf("           bss : %p - %p\n", &__bss_start__, &__bss_end__);

      bytes = (&__bss2_end__ - &__bss2_start__);
      printf("      RAM2 : %d (%2d %%)\n", bytes, bytes * 100 / (32 * 1024));
      printf("           bss2: %p - %p\n", &__bss2_start__, &__bss2_end__);

      if (Lpc17xxRTC::valueOk()) {
         Lpc17xxRTC::gettime(&ns);
         sec = ns / 1E9;
         //tm = *(gmtime(&sec));
         tm = *(localtime(&sec));
         printf("RTC is active: ");
         printf(ctime(&sec));
         printf("------------\n");
      } else {
         printf("RTC has ridiculous time\n");
         Lpc17xxRTC::set(&defaultDate);
      }

      while (!Lpc17xxRTC::isRunning()) {
         printf("RTC is not running --> start it\n");
         Lpc17xxRTC::start();

         if (!Lpc17xxRTC::isRunning()) {
            printf("RTC start was not successful --> hardware problem?"
                   " try again\n");
         }
      }
   }

   void Post::config(void) {
      struct tm tm;
      char line[80];

      while (1) {
         printf("commands: \n");
         printf("RTC: DOW YYYY:MM:DD:HH:MIN:SEC  set full date (DOW:1=Mon)\n");
         printf("RUN                             start application\n");

         fgets(line, sizeof(line) - 1, stdin);

         if (7 == sscanf(line, "RTC: %d %d:%d:%d:%d:%d:%d",
                         &tm.tm_wday,
                         &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                         &tm.tm_hour, &tm.tm_min, &tm.tm_sec)) {
            tm.tm_mon--;
            tm.tm_year -= 1900;
            printf(" set RTC to: ");
            printf(asctime(&tm));
            pearlrt::Lpc17xxRTC rtc;
            rtc.set(&tm);
         } else if (strncmp(line, "RUN", 3) == 0) {
            printf("exit POST ... run application\n");
            return;
         } else {
            printf("???? unknown command\n");
         }
      }
   }
};

#include <chip.h>
#include <rtc_17xx_40xx.h>
#include <stdio.h>
#include <string.h>
#include "Post.h"

// labels from the linker script
extern "C" {
// lables defined as char to get the difference in bytes instead of ints
   extern unsigned char __etext;
   extern unsigned char __data_start__, __data_end__;
   extern unsigned char __bss_start__, __bss_end__;
};

void Post::print(void) {
   int bytes;
   RTC_TIME_T tm;

   printf("POST summary\n");
   printf("  main clock rate: %lu MHz\n",
          Chip_Clock_GetSystemClockRate() / 1000000);
   printf("  Flash usage:\n");
   printf("        code: 0x%p\n", &__etext);
   printf("  data(copy): 0x%x \n", &__data_end__ - &__data_start__);
   bytes = (int)(&__etext) + (__data_end__ - __data_start__);
   printf("      total: %d (%2d %%)\n", bytes, bytes * 100 / (512 * 1024));

   printf("  RAM usage:\n");
   printf("      data: 0x%p - 0x%p \n", &__data_start__, &__data_end__);
   printf("      bss : 0x%p - 0x%p\n", &__bss_start__, &__bss_end__);
   bytes = (&__data_end__ - &__data_start__) +
           (&__bss_end__ - &__bss_start__);
   printf("      total: %d (%2d %%)\n", bytes, bytes * 100 / (32 * 1024));

   Chip_RTC_GetFullTime(LPC_RTC, &tm);
   printf("  RTC: DOY=%d  DOW=%d %02d.%02d.%04d:%02d:%02d:%02d\n",
          (int)tm.time[RTC_TIMETYPE_DAYOFYEAR],
          (int)tm.time[RTC_TIMETYPE_DAYOFWEEK],
          (int)tm.time[RTC_TIMETYPE_DAYOFMONTH],
          (int)tm.time[RTC_TIMETYPE_MONTH],
          (int)tm.time[RTC_TIMETYPE_YEAR],
          (int)tm.time[RTC_TIMETYPE_HOUR],
          (int)tm.time[RTC_TIMETYPE_MINUTE],
          (int)tm.time[RTC_TIMETYPE_SECOND]);
}
void Post::config(void) {
   RTC_TIME_T tm;
   char line[80];

   while (1) {
      printf("commands: \n");
      printf("RTC: DOY:DOW:DD:MM:YYYY:HH:MIN:SEC  set full date \n");
      printf("RUN                                 run application \n");

      fgets(line, sizeof(line) - 1, stdin);

      if (8 == sscanf(line, "RTC:%d:%d:%d:%d:%d:%d:%d:%d",
                      (int*)&tm.time[RTC_TIMETYPE_DAYOFYEAR],
                      (int*)&tm.time[RTC_TIMETYPE_DAYOFWEEK],
                      (int*)&tm.time[RTC_TIMETYPE_DAYOFMONTH],
                      (int*)&tm.time[RTC_TIMETYPE_MONTH],
                      (int*)&tm.time[RTC_TIMETYPE_YEAR],
                      (int*)&tm.time[RTC_TIMETYPE_HOUR],
                      (int*)&tm.time[RTC_TIMETYPE_MINUTE],
                      (int*)&tm.time[RTC_TIMETYPE_SECOND])) {
         printf(" set RTC to: DOY=%d  DOW=%d %02d.%02d.%04d:%02d:%02d:%02d\n",
                (int)tm.time[RTC_TIMETYPE_DAYOFYEAR],
                (int)tm.time[RTC_TIMETYPE_DAYOFWEEK],
                (int)tm.time[RTC_TIMETYPE_DAYOFMONTH],
                (int)tm.time[RTC_TIMETYPE_MONTH],
                (int)tm.time[RTC_TIMETYPE_YEAR],
                (int)tm.time[RTC_TIMETYPE_HOUR],
                (int)tm.time[RTC_TIMETYPE_MINUTE],
                (int)tm.time[RTC_TIMETYPE_SECOND]);
         Chip_RTC_SetFullTime(LPC_RTC, &tm);

      } else if (strncmp(line, "RUN", 3) == 0) {
         printf("exit POST ... run application\n");
         return;
      } else {
         printf("???? unknown command\n");
      }
   }
}

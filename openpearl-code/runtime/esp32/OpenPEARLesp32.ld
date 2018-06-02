/* ESP32 Linker Script Memory Layout


   ***

   This linker script is passed through the C preprocessor to include
   configuration options.

   Please use preprocessor features sparingly! Restrict
   to simple macros with numeric values, and/or #if/#endif blocks.
*/

/* BT is not built at all */

MEMORY
{
  /* All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
  are connected to the data port of the CPU and eg allow bytewise access. */

  /* IRAM for PRO cpu. Not sure if happy with this, this is MMU area... */
  iram0_0_seg (RX) :                 org = 0x40080000, len = 0x20000

  /* Even though the segment name is iram, it is actually mapped to flash
  */
  iram0_2_seg (RX) :                 org = 0x400D0018, len = 0x330000-0x18

  /*
    (0x18 offset above is a convenience for the app binary image generation. Flash cache has 64KB pages. The .bin file
    which is flashed to the chip has a 0x18 byte file header. Setting this offset makes it simple to meet the flash
    cache MMU's constraint that (paddr % 64KB == vaddr % 64KB).)
  */


  /* Shared data RAM, excluding memory reserved for ROM bss/data/stack.

     Enabling Bluetooth & Trace Memory features in menuconfig will decrease
     the amount of RAM available.

     Note: Length of this section *should* be 0x50000, and this extra DRAM is available
     in heap at runtime. However due to static ROM memory usage at this 176KB mark, the
     additional static memory temporarily cannot be used.
  */
  dram0_0_seg (RW) :                 org = 0x3FFB0000,
                                     len = 0x2c200

  /* Flash mapped constant data */
  drom0_0_seg (R) :                  org = 0x3F400018, len = 0x400000-0x18

  /* (See iram0_2_seg for meaning of 0x18 offset in the above.) */

  /* RTC fast memory (executable). Persists over deep sleep.
   */
  rtc_iram_seg(RWX) :                org = 0x400C0000, len = 0x2000

  /* RTC slow memory (data accessible). Persists over deep sleep.

     Start of RTC slow memory is reserved for ULP co-processor code + data, if enabled.
  */
  rtc_slow_seg(RW)  :                org = 0x50000000,
                                     len = 0x1000
}

/* Heap ends at top of dram0_0_seg */
_heap_end = 0x40000000;



/*  Default entry point:  */
ENTRY(call_start_cpu0);

SECTIONS
{
  /* RTC fast memory holds RTC wake stub code,
     including from any source file named rtc_wake_stub*.c
  */
  .rtc.text :
  {
    . = ALIGN(4);
    *(.rtc.literal .rtc.text)
    *rtc_wake_stub*.o(.literal .text .literal.* .text.*)
  } >rtc_iram_seg

  /* RTC slow memory holds RTC wake stub
     data/rodata, including from any source file
     named rtc_wake_stub*.c
  */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);
    *(.rtc.data)
    *(.rtc.rodata)
    *rtc_wake_stub*.o(.data .rodata .data.* .rodata.* .bss .bss.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.o(.bss .bss.*)
    *rtc_wake_stub*.o(COMMON)
    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    /* Vectors go to IRAM */
    _init_start = ABSOLUTE(.);
    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);

    /* This goes here, not at top of linker script, so addr2line finds it last,
       and uses it in preference to the first symbol in IRAM */
    _iram_start = ABSOLUTE(0);
  } > iram0_0_seg

  .iram0.text :
  {
    /* Code marked as runnning out of IRAM */
    _iram_text_start = ABSOLUTE(.);
    *(.iram1 .iram1.*)
    *libfreertos.a:(.literal .text .literal.* .text.*)
    *libheap.a:multi_heap.o(.literal .text .literal.* .text.*)
    *libheap.a:multi_heap_poisoning.o(.literal .text .literal.* .text.*)
    *libesp32.a:panic.o(.literal .text .literal.* .text.*)
    *libesp32.a:core_dump.o(.literal .text .literal.* .text.*)
    *libapp_trace.a:(.literal .text .literal.* .text.*)
    *libxtensa-debug-module.a:eri.o(.literal .text .literal.* .text.*)
    *librtc.a:(.literal .text .literal.* .text.*)
    *libsoc.a:(.literal .text .literal.* .text.*)
    *libhal.a:(.literal .text .literal.* .text.*)
    *libgcc.a:lib2funcs.o(.literal .text .literal.* .text.*)
    *libspi_flash.a:spi_flash_rom_patch.o(.literal .text .literal.* .text.*)
    *libgcov.a:(.literal .text .literal.* .text.*)
    /*
 If the Newlib functions in ROM aren't used (eg because the external SPI RAM workaround is active), these functions will 
 be linked into the application directly instead. Normally, they would end up in flash, which is undesirable because esp-idf
 and/or applications may assume that because these functions normally are in ROM, they are accessible even when flash is 
 inaccessible. To work around this, this ld fragment places these functions in RAM instead. If the ROM functions are used,
 these defines do nothing, so they can still be included in that situation.

 This file is responsible for placing the literal and text segments in IRAM.
*/


    *lib_a-utoa.o(.literal .text .literal.* .text.*)
    *lib_a-longjmp.o(.literal .text .literal.* .text.*)
    *lib_a-setjmp.o(.literal .text .literal.* .text.*)
    *lib_a-abs.o(.literal .text .literal.* .text.*)
    *lib_a-div.o(.literal .text .literal.* .text.*)
    *lib_a-labs.o(.literal .text .literal.* .text.*)
    *lib_a-ldiv.o(.literal .text .literal.* .text.*)
    *lib_a-quorem.o(.literal .text .literal.* .text.*)
    *lib_a-qsort.o(.literal .text .literal.* .text.*)
    *lib_a-utoa.o(.literal .text .literal.* .text.*)
    *lib_a-itoa.o(.literal .text .literal.* .text.*)
    *lib_a-atoi.o(.literal .text .literal.* .text.*)
    *lib_a-atol.o(.literal .text .literal.* .text.*)
    *lib_a-strtol.o(.literal .text .literal.* .text.*)
    *lib_a-strtoul.o(.literal .text .literal.* .text.*)
    *lib_a-wcrtomb.o(.literal .text .literal.* .text.*)
    *lib_a-fvwrite.o(.literal .text .literal.* .text.*)
    *lib_a-wbuf.o(.literal .text .literal.* .text.*)
    *lib_a-wsetup.o(.literal .text .literal.* .text.*)
    *lib_a-fputwc.o(.literal .text .literal.* .text.*)
    *lib_a-wctomb_r.o(.literal .text .literal.* .text.*)
    *lib_a-ungetc.o(.literal .text .literal.* .text.*)
    *lib_a-makebuf.o(.literal .text .literal.* .text.*)
    *lib_a-fflush.o(.literal .text .literal.* .text.*)
    *lib_a-refill.o(.literal .text .literal.* .text.*)
    *lib_a-s_fpclassify.o(.literal .text .literal.* .text.*)
    *lib_a-locale.o(.literal .text .literal.* .text.*)
    *lib_a-asctime.o(.literal .text .literal.* .text.*)
    *lib_a-ctime.o(.literal .text .literal.* .text.*)
    *lib_a-ctime_r.o(.literal .text .literal.* .text.*)
    *lib_a-lcltime.o(.literal .text .literal.* .text.*)
    *lib_a-lcltime_r.o(.literal .text .literal.* .text.*)
    *lib_a-gmtime.o(.literal .text .literal.* .text.*)
    *lib_a-gmtime_r.o(.literal .text .literal.* .text.*)
    *lib_a-strftime.o(.literal .text .literal.* .text.*)
    *lib_a-mktime.o(.literal .text .literal.* .text.*)
    *lib_a-syswrite.o(.literal .text .literal.* .text.*)
    *lib_a-tzset_r.o(.literal .text .literal.* .text.*)
    *lib_a-tzset.o(.literal .text .literal.* .text.*)
    *lib_a-toupper.o(.literal .text .literal.* .text.*)
    *lib_a-tolower.o(.literal .text .literal.* .text.*)
    *lib_a-toascii.o(.literal .text .literal.* .text.*)
    *lib_a-systimes.o(.literal .text .literal.* .text.*)
    *lib_a-time.o(.literal .text .literal.* .text.*)
    *lib_a-bsd_qsort_r.o(.literal .text .literal.* .text.*)
    *lib_a-qsort_r.o(.literal .text .literal.* .text.*)
    *lib_a-gettzinfo.o(.literal .text .literal.* .text.*)
    *lib_a-strupr.o(.literal .text .literal.* .text.*)
    *lib_a-asctime_r.o(.literal .text .literal.* .text.*)
    *lib_a-bzero.o(.literal .text .literal.* .text.*)
    *lib_a-close.o(.literal .text .literal.* .text.*)
    *lib_a-creat.o(.literal .text .literal.* .text.*)
    *lib_a-environ.o(.literal .text .literal.* .text.*)
    *lib_a-fclose.o(.literal .text .literal.* .text.*)
    *lib_a-isalnum.o(.literal .text .literal.* .text.*)
    *lib_a-isalpha.o(.literal .text .literal.* .text.*)
    *lib_a-isascii.o(.literal .text .literal.* .text.*)
    *lib_a-isblank.o(.literal .text .literal.* .text.*)
    *lib_a-iscntrl.o(.literal .text .literal.* .text.*)
    *lib_a-isdigit.o(.literal .text .literal.* .text.*)
    *lib_a-isgraph.o(.literal .text .literal.* .text.*)
    *lib_a-islower.o(.literal .text .literal.* .text.*)
    *lib_a-isprint.o(.literal .text .literal.* .text.*)
    *lib_a-ispunct.o(.literal .text .literal.* .text.*)
    *lib_a-isspace.o(.literal .text .literal.* .text.*)
    *lib_a-isupper.o(.literal .text .literal.* .text.*)
    *lib_a-memccpy.o(.literal .text .literal.* .text.*)
    *lib_a-memchr.o(.literal .text .literal.* .text.*)
    *lib_a-memcmp.o(.literal .text .literal.* .text.*)
    *lib_a-memcpy.o(.literal .text .literal.* .text.*)
    *lib_a-memmove.o(.literal .text .literal.* .text.*)
    *lib_a-memrchr.o(.literal .text .literal.* .text.*)
    *lib_a-memset.o(.literal .text .literal.* .text.*)
    *lib_a-open.o(.literal .text .literal.* .text.*)
    *lib_a-rand.o(.literal .text .literal.* .text.*)
    *lib_a-rand_r.o(.literal .text .literal.* .text.*)
    *lib_a-read.o(.literal .text .literal.* .text.*)
    *lib_a-rshift.o(.literal .text .literal.* .text.*)
    *lib_a-sbrk.o(.literal .text .literal.* .text.*)
    *lib_a-srand.o(.literal .text .literal.* .text.*)
    *lib_a-strcasecmp.o(.literal .text .literal.* .text.*)
    *lib_a-strcasestr.o(.literal .text .literal.* .text.*)
    *lib_a-strcat.o(.literal .text .literal.* .text.*)
    *lib_a-strchr.o(.literal .text .literal.* .text.*)
    *lib_a-strcmp.o(.literal .text .literal.* .text.*)
    *lib_a-strcoll.o(.literal .text .literal.* .text.*)
    *lib_a-strcpy.o(.literal .text .literal.* .text.*)
    *lib_a-strcspn.o(.literal .text .literal.* .text.*)
    *lib_a-strdup.o(.literal .text .literal.* .text.*)
    *lib_a-strlcat.o(.literal .text .literal.* .text.*)
    *lib_a-strlcpy.o(.literal .text .literal.* .text.*)
    *lib_a-strlen.o(.literal .text .literal.* .text.*)
    *lib_a-strlwr.o(.literal .text .literal.* .text.*)
    *lib_a-strncasecmp.o(.literal .text .literal.* .text.*)
    *lib_a-strncat.o(.literal .text .literal.* .text.*)
    *lib_a-strncmp.o(.literal .text .literal.* .text.*)
    *lib_a-strncpy.o(.literal .text .literal.* .text.*)
    *lib_a-strndup.o(.literal .text .literal.* .text.*)
    *lib_a-strnlen.o(.literal .text .literal.* .text.*)
    *lib_a-strrchr.o(.literal .text .literal.* .text.*)
    *lib_a-strsep.o(.literal .text .literal.* .text.*)
    *lib_a-strspn.o(.literal .text .literal.* .text.*)
    *lib_a-strstr.o(.literal .text .literal.* .text.*)
    *lib_a-strtok_r.o(.literal .text .literal.* .text.*)
    *lib_a-strupr.o(.literal .text .literal.* .text.*)
    *lib_a-stdio.o(.literal .text .literal.* .text.*)
    *lib_a-syssbrk.o(.literal .text .literal.* .text.*)
    *lib_a-sysclose.o(.literal .text .literal.* .text.*)
    *lib_a-sysopen.o(.literal .text .literal.* .text.*)
    *creat.o(.literal .text .literal.* .text.*)
    *lib_a-sysread.o(.literal .text .literal.* .text.*)
    *lib_a-syswrite.o(.literal .text .literal.* .text.*)
    *lib_a-impure.o(.literal .text .literal.* .text.*)
    *lib_a-tzvars.o(.literal .text .literal.* .text.*)
    *lib_a-sf_nan.o(.literal .text .literal.* .text.*)
    *lib_a-tzcalc_limits.o(.literal .text .literal.* .text.*)
    *lib_a-month_lengths.o(.literal .text .literal.* .text.*)
    *lib_a-timelocal.o(.literal .text .literal.* .text.*)
    *lib_a-findfp.o(.literal .text .literal.* .text.*)
    *lock.o(.literal .text .literal.* .text.*)
    *lib_a-getenv_r.o(.literal .text .literal.* .text.*)
    *isatty.o(.literal .text .literal.* .text.*)
    *lib_a-fwalk.o(.literal .text .literal.* .text.*)
    *lib_a-getenv_r.o(.literal .text .literal.* .text.*)
    *lib_a-tzlock.o(.literal .text .literal.* .text.*)
    *lib_a-ctype_.o(.literal .text .literal.* .text.*)
    *lib_a-sccl.o(.literal .text .literal.* .text.*)
    *lib_a-strptime.o(.literal .text .literal.* .text.*)
    *lib_a-envlock.o(.literal .text .literal.* .text.*)
    *lib_a-raise.o(.literal .text .literal.* .text.*)
    *lib_a-strdup_r.o(.literal .text .literal.* .text.*)
    *lib_a-system.o(.literal .text .literal.* .text.*)
    *lib_a-strndup_r.o(.literal .text .literal.* .text.*)
    _iram_text_end = ABSOLUTE(.);
  } > iram0_0_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    *(.dram1 .dram1.*)
    *libesp32.a:panic.o(.rodata .rodata.*)
    *libphy.a:(.rodata .rodata.*)
    *libsoc.a:rtc_clk.o(.rodata .rodata.*)
    *libapp_trace.a:(.rodata .rodata.*)
    *libgcov.a:(.rodata .rodata.*)
    *libheap.a:multi_heap.o(.rodata .rodata.*)
    *libheap.a:multi_heap_poisoning.o(.rodata .rodata.*)
    /*
 If the Newlib functions in ROM aren't used (eg because the external SPI RAM workaround is active), these functions will 
 be linked into the application directly instead. Normally, they would end up in flash, which is undesirable because esp-idf
 and/or applications may assume that because these functions normally are in ROM, they are accessible even when flash is 
 inaccessible. To work around this, this ld fragment places these functions in RAM instead. If the ROM functions are used,
 these defines do nothing, so they can still be included in that situation.

 This file is responsible for placing the rodata segment in DRAM.
*/

    *lib_a-utoa.o(.rodata .rodata.*)
    *lib_a-longjmp.o(.rodata .rodata.*)
    *lib_a-setjmp.o(.rodata .rodata.*)
    *lib_a-abs.o(.rodata .rodata.*)
    *lib_a-div.o(.rodata .rodata.*)
    *lib_a-labs.o(.rodata .rodata.*)
    *lib_a-ldiv.o(.rodata .rodata.*)
    *lib_a-quorem.o(.rodata .rodata.*)
    *lib_a-qsort.o(.rodata .rodata.*)
    *lib_a-utoa.o(.rodata .rodata.*)
    *lib_a-itoa.o(.rodata .rodata.*)
    *lib_a-atoi.o(.rodata .rodata.*)
    *lib_a-atol.o(.rodata .rodata.*)
    *lib_a-strtol.o(.rodata .rodata.*)
    *lib_a-strtoul.o(.rodata .rodata.*)
    *lib_a-wcrtomb.o(.rodata .rodata.*)
    *lib_a-fvwrite.o(.rodata .rodata.*)
    *lib_a-wbuf.o(.rodata .rodata.*)
    *lib_a-wsetup.o(.rodata .rodata.*)
    *lib_a-fputwc.o(.rodata .rodata.*)
    *lib_a-wctomb_r.o(.rodata .rodata.*)
    *lib_a-ungetc.o(.rodata .rodata.*)
    *lib_a-makebuf.o(.rodata .rodata.*)
    *lib_a-fflush.o(.rodata .rodata.*)
    *lib_a-refill.o(.rodata .rodata.*)
    *lib_a-s_fpclassify.o(.rodata .rodata.*)
    *lib_a-locale.o(.rodata .rodata.*)
    *lib_a-asctime.o(.rodata .rodata.*)
    *lib_a-ctime.o(.rodata .rodata.*)
    *lib_a-ctime_r.o(.rodata .rodata.*)
    *lib_a-lcltime.o(.rodata .rodata.*)
    *lib_a-lcltime_r.o(.rodata .rodata.*)
    *lib_a-gmtime.o(.rodata .rodata.*)
    *lib_a-gmtime_r.o(.rodata .rodata.*)
    *lib_a-strftime.o(.rodata .rodata.*)
    *lib_a-mktime.o(.rodata .rodata.*)
    *lib_a-syswrite.o(.rodata .rodata.*)
    *lib_a-tzset_r.o(.rodata .rodata.*)
    *lib_a-tzset.o(.rodata .rodata.*)
    *lib_a-toupper.o(.rodata .rodata.*)
    *lib_a-tolower.o(.rodata .rodata.*)
    *lib_a-toascii.o(.rodata .rodata.*)
    *lib_a-systimes.o(.rodata .rodata.*)
    *lib_a-time.o(.rodata .rodata.*)
    *lib_a-bsd_qsort_r.o(.rodata .rodata.*)
    *lib_a-qsort_r.o(.rodata .rodata.*)
    *lib_a-gettzinfo.o(.rodata .rodata.*)
    *lib_a-strupr.o(.rodata .rodata.*)
    *lib_a-asctime_r.o(.rodata .rodata.*)
    *lib_a-bzero.o(.rodata .rodata.*)
    *lib_a-close.o(.rodata .rodata.*)
    *lib_a-creat.o(.rodata .rodata.*)
    *lib_a-environ.o(.rodata .rodata.*)
    *lib_a-fclose.o(.rodata .rodata.*)
    *lib_a-isalnum.o(.rodata .rodata.*)
    *lib_a-isalpha.o(.rodata .rodata.*)
    *lib_a-isascii.o(.rodata .rodata.*)
    *lib_a-isblank.o(.rodata .rodata.*)
    *lib_a-iscntrl.o(.rodata .rodata.*)
    *lib_a-isdigit.o(.rodata .rodata.*)
    *lib_a-isgraph.o(.rodata .rodata.*)
    *lib_a-islower.o(.rodata .rodata.*)
    *lib_a-isprint.o(.rodata .rodata.*)
    *lib_a-ispunct.o(.rodata .rodata.*)
    *lib_a-isspace.o(.rodata .rodata.*)
    *lib_a-isupper.o(.rodata .rodata.*)
    *lib_a-memccpy.o(.rodata .rodata.*)
    *lib_a-memchr.o(.rodata .rodata.*)
    *lib_a-memcmp.o(.rodata .rodata.*)
    *lib_a-memcpy.o(.rodata .rodata.*)
    *lib_a-memmove.o(.rodata .rodata.*)
    *lib_a-memrchr.o(.rodata .rodata.*)
    *lib_a-memset.o(.rodata .rodata.*)
    *lib_a-open.o(.rodata .rodata.*)
    *lib_a-rand.o(.rodata .rodata.*)
    *lib_a-rand_r.o(.rodata .rodata.*)
    *lib_a-read.o(.rodata .rodata.*)
    *lib_a-rshift.o(.rodata .rodata.*)
    *lib_a-sbrk.o(.rodata .rodata.*)
    *lib_a-srand.o(.rodata .rodata.*)
    *lib_a-strcasecmp.o(.rodata .rodata.*)
    *lib_a-strcasestr.o(.rodata .rodata.*)
    *lib_a-strcat.o(.rodata .rodata.*)
    *lib_a-strchr.o(.rodata .rodata.*)
    *lib_a-strcmp.o(.rodata .rodata.*)
    *lib_a-strcoll.o(.rodata .rodata.*)
    *lib_a-strcpy.o(.rodata .rodata.*)
    *lib_a-strcspn.o(.rodata .rodata.*)
    *lib_a-strdup.o(.rodata .rodata.*)
    *lib_a-strlcat.o(.rodata .rodata.*)
    *lib_a-strlcpy.o(.rodata .rodata.*)
    *lib_a-strlen.o(.rodata .rodata.*)
    *lib_a-strlwr.o(.rodata .rodata.*)
    *lib_a-strncasecmp.o(.rodata .rodata.*)
    *lib_a-strncat.o(.rodata .rodata.*)
    *lib_a-strncmp.o(.rodata .rodata.*)
    *lib_a-strncpy.o(.rodata .rodata.*)
    *lib_a-strndup.o(.rodata .rodata.*)
    *lib_a-strnlen.o(.rodata .rodata.*)
    *lib_a-strrchr.o(.rodata .rodata.*)
    *lib_a-strsep.o(.rodata .rodata.*)
    *lib_a-strspn.o(.rodata .rodata.*)
    *lib_a-strstr.o(.rodata .rodata.*)
    *lib_a-strtok_r.o(.rodata .rodata.*)
    *lib_a-strupr.o(.rodata .rodata.*)
    *lib_a-stdio.o(.rodata .rodata.*)
    *lib_a-syssbrk.o(.rodata .rodata.*)
    *lib_a-sysclose.o(.rodata .rodata.*)
    *lib_a-sysopen.o(.rodata .rodata.*)
    *creat.o(.rodata .rodata.*)
    *lib_a-sysread.o(.rodata .rodata.*)
    *lib_a-syswrite.o(.rodata .rodata.*)
    *lib_a-impure.o(.rodata .rodata.*)
    *lib_a-tzvars.o(.rodata .rodata.*)
    *lib_a-sf_nan.o(.rodata .rodata.*)
    *lib_a-tzcalc_limits.o(.rodata .rodata.*)
    *lib_a-month_lengths.o(.rodata .rodata.*)
    *lib_a-timelocal.o(.rodata .rodata.*)
    *lib_a-findfp.o(.rodata .rodata.*)
    *lock.o(.rodata .rodata.*)
    *lib_a-getenv_r.o(.rodata .rodata.*)
    *isatty.o(.rodata .rodata.*)
    *lib_a-fwalk.o(.rodata .rodata.*)
    *lib_a-getenv_r.o(.rodata .rodata.*)
    *lib_a-tzlock.o(.rodata .rodata.*)
    *lib_a-ctype_.o(.rodata .rodata.*)
    *lib_a-sccl.o(.rodata .rodata.*)
    *lib_a-strptime.o(.rodata .rodata.*)
    *lib_a-envlock.o(.rodata .rodata.*)
    *lib_a-raise.o(.rodata .rodata.*)
    *lib_a-strdup_r.o(.rodata .rodata.*)
    *lib_a-system.o(.rodata .rodata.*)
    *lib_a-strndup_r.o(.rodata .rodata.*)
    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >dram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.share.mem)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _heap_start = ABSOLUTE(.);
  } >dram0_0_seg

  .flash.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    /*  C++ constructor and destructor tables, properly ordered:  */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >drom0_0_seg

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;

    /* Similar to _iram_start, this symbol goes here so it is
       resolved by addr2line in preference to the first symbol in
       the flash.text segment.
    */
    _flash_cache_start = ABSOLUTE(0);
  } >iram0_2_seg
}

PROVIDE ( UART0 = 0x3ff40000 );
PROVIDE ( SPI1 = 0x3ff42000 );
PROVIDE ( SPI0 = 0x3ff43000 );
PROVIDE ( GPIO = 0x3ff44000 );
PROVIDE ( SIGMADELTA = 0x3ff44f00 );
PROVIDE ( RTCCNTL = 0x3ff48000 );
PROVIDE ( RTCIO = 0x3ff48400 );
PROVIDE ( SENS = 0x3ff48800 );
PROVIDE ( UHCI1 = 0x3ff4C000 );
PROVIDE ( I2S0 = 0x3ff4F000 );
PROVIDE ( UART1 = 0x3ff50000 );
PROVIDE ( I2C0 = 0x3ff53000 );
PROVIDE ( UHCI0 = 0x3ff54000 );
PROVIDE ( RMT = 0x3ff56000 );
PROVIDE ( RMTMEM = 0x3ff56800 );
PROVIDE ( PCNT = 0x3ff57000 );
PROVIDE ( LEDC = 0x3ff59000 );
PROVIDE ( MCPWM0 = 0x3ff5E000 );
PROVIDE ( TIMERG0 = 0x3ff5F000 );
PROVIDE ( TIMERG1 = 0x3ff60000 );
PROVIDE ( SPI2 = 0x3ff64000 );
PROVIDE ( SPI3 = 0x3ff65000 );
PROVIDE ( SYSCON = 0x3ff66000 );
PROVIDE ( I2C1 = 0x3ff67000 );
PROVIDE ( SDMMC = 0x3ff68000 );
PROVIDE ( MCPWM1 = 0x3ff6C000 );
PROVIDE ( I2S1 = 0x3ff6D000 );
PROVIDE ( UART2 = 0x3ff6E000 );

/*
 If the spiram compiler workaround is active, these functions from ROM cannot be used. If the workaround is not
 active (e.g. because the system does not use SPI RAM) then these functions are okay to use.
*/
PROVIDE ( abs = 0x40056340 );
PROVIDE ( __ascii_wctomb = 0x40058ef0 );
PROVIDE ( asctime = 0x40059588 );
PROVIDE ( asctime_r = 0x40000ec8 );
PROVIDE ( atoi = 0x400566c4 );
PROVIDE ( _atoi_r = 0x400566d4 );
PROVIDE ( atol = 0x400566ec );
PROVIDE ( _atol_r = 0x400566fc );
PROVIDE ( bzero = 0x4000c1f4 );
PROVIDE ( _cleanup = 0x40001df8 );
PROVIDE ( _cleanup_r = 0x40001d48 );
PROVIDE ( close = 0x40001778 );
PROVIDE ( creat = 0x40000e8c );
PROVIDE ( ctime = 0x400595b0 );
PROVIDE ( ctime_r = 0x400595c4 );
PROVIDE ( div = 0x40056348 );
PROVIDE ( __dummy_lock = 0x4000c728 );
PROVIDE ( __dummy_lock_try = 0x4000c730 );
PROVIDE ( __env_lock = 0x40001fd4 );
PROVIDE ( __env_unlock = 0x40001fe0 );
PROVIDE ( fclose = 0x400020ac );
PROVIDE ( _fclose_r = 0x40001fec );
PROVIDE ( fflush = 0x40059394 );
PROVIDE ( _fflush_r = 0x40059320 );
PROVIDE ( _findenv_r = 0x40001f44 );
PROVIDE ( __fp_lock_all = 0x40001f1c );
PROVIDE ( __fp_unlock_all = 0x40001f30 );
PROVIDE ( fputwc = 0x40058ea8 );
PROVIDE ( __fputwc = 0x40058da0 );
PROVIDE ( _fputwc_r = 0x40058e4c );
PROVIDE ( _fwalk = 0x4000c738 );
PROVIDE ( _fwalk_reent = 0x4000c770 );
PROVIDE ( __get_current_time_locale = 0x40001834 );
PROVIDE ( _getenv_r = 0x40001fbc );
PROVIDE ( __gettzinfo = 0x40001fcc );
PROVIDE ( gmtime = 0x40059848 );
PROVIDE ( gmtime_r = 0x40059868 );
PROVIDE ( isalnum = 0x40000f04 );
PROVIDE ( isalpha = 0x40000f18 );
PROVIDE ( isascii = 0x4000c20c );
PROVIDE ( _isatty_r = 0x40000ea0 );
PROVIDE ( isblank = 0x40000f2c );
PROVIDE ( iscntrl = 0x40000f50 );
PROVIDE ( isdigit = 0x40000f64 );
PROVIDE ( isgraph = 0x40000f94 );
PROVIDE ( islower = 0x40000f78 );
PROVIDE ( isprint = 0x40000fa8 );
PROVIDE ( ispunct = 0x40000fc0 );
PROVIDE ( isspace = 0x40000fd4 );
PROVIDE ( isupper = 0x40000fe8 );
PROVIDE ( itoa = 0x400566b4 );
PROVIDE ( __itoa = 0x40056678 );
PROVIDE ( labs = 0x40056370 );
PROVIDE ( ldiv = 0x40056378 );
PROVIDE ( __locale_charset = 0x40059540 );
PROVIDE ( __locale_cjk_lang = 0x40059558 );
PROVIDE ( localeconv = 0x4005957c );
PROVIDE ( _localeconv_r = 0x40059560 );
PROVIDE ( __locale_mb_cur_max = 0x40059548 );
PROVIDE ( __locale_msgcharset = 0x40059550 );
PROVIDE ( localtime = 0x400595dc );
PROVIDE ( localtime_r = 0x400595fc );
PROVIDE ( _lock_acquire = 0x4000be14 );
PROVIDE ( _lock_release = 0x4000be64 );
PROVIDE ( longjmp = 0x400562cc );
PROVIDE ( memccpy = 0x4000c220 );
PROVIDE ( memchr = 0x4000c244 );
PROVIDE ( memcmp = 0x4000c260 );
PROVIDE ( memcpy = 0x4000c2c8 );
PROVIDE ( memmove = 0x4000c3c0 );
PROVIDE ( memrchr = 0x4000c400 );
PROVIDE ( memset = 0x4000c44c );
PROVIDE ( mktime = 0x4005a5e8 );
PROVIDE ( open = 0x4000178c );
PROVIDE ( qsort = 0x40056424 );
PROVIDE ( _raise_r = 0x4000bc70 );
PROVIDE ( rand = 0x40001058 );
PROVIDE ( rand_r = 0x400010d4 );
PROVIDE ( read = 0x400017dc );
PROVIDE ( sbrk = 0x400017f4 );
PROVIDE ( __sccl = 0x4000c498 );
PROVIDE ( __sclose = 0x400011b8 );
PROVIDE ( __seofread = 0x40001148 );
PROVIDE ( setjmp = 0x40056268 );
PROVIDE ( setlocale = 0x40059568 );
PROVIDE ( _setlocale_r = 0x4005950c );
PROVIDE ( __sflush_r = 0x400591e0 );
PROVIDE ( __sfmoreglue = 0x40001dc8 );
PROVIDE ( __sfp = 0x40001e90 );
PROVIDE ( __sfp_lock_acquire = 0x40001e08 );
PROVIDE ( __sfp_lock_release = 0x40001e14 );
PROVIDE ( __sfputs_r = 0x40057790 );
PROVIDE ( __sfvwrite_r = 0x4005893c );
PROVIDE ( __sinit = 0x40001e38 );
PROVIDE ( __sinit_lock_acquire = 0x40001e20 );
PROVIDE ( __sinit_lock_release = 0x40001e2c );
PROVIDE ( __smakebuf_r = 0x40059108 );
PROVIDE ( srand = 0x40001004 );
PROVIDE ( __sread = 0x40001118 );
PROVIDE ( __srefill_r = 0x400593d4 );
PROVIDE ( __sseek = 0x40001184 );
PROVIDE ( __ssprint_r = 0x40056ff8 );
PROVIDE ( __ssputs_r = 0x40056f2c );
PROVIDE ( __ssrefill_r = 0x40057fec );
PROVIDE ( strcasecmp = 0x400011cc );
PROVIDE ( strcasestr = 0x40001210 );
PROVIDE ( strcat = 0x4000c518 );
PROVIDE ( strchr = 0x4000c53c );
PROVIDE ( strcmp = 0x40001274 );
PROVIDE ( strcoll = 0x40001398 );
PROVIDE ( strcpy = 0x400013ac );
PROVIDE ( strcspn = 0x4000c558 );
PROVIDE ( strdup = 0x4000143c );
PROVIDE ( _strdup_r = 0x40001450 );
PROVIDE ( strftime = 0x40059ab4 );
PROVIDE ( strlcat = 0x40001470 );
PROVIDE ( strlcpy = 0x4000c584 );
PROVIDE ( strlen = 0x400014c0 );
PROVIDE ( strlwr = 0x40001524 );
PROVIDE ( strncasecmp = 0x40001550 );
PROVIDE ( strncat = 0x4000c5c4 );
PROVIDE ( strncmp = 0x4000c5f4 );
PROVIDE ( strncpy = 0x400015d4 );
PROVIDE ( strndup = 0x400016b0 );
PROVIDE ( _strndup_r = 0x400016c4 );
PROVIDE ( strnlen = 0x4000c628 );
PROVIDE ( strrchr = 0x40001708 );
PROVIDE ( strsep = 0x40001734 );
PROVIDE ( strspn = 0x4000c648 );
PROVIDE ( strstr = 0x4000c674 );
PROVIDE ( __strtok_r = 0x4000c6a8 );
PROVIDE ( strtok_r = 0x4000c70c );
PROVIDE ( strtol = 0x4005681c );
PROVIDE ( _strtol_r = 0x40056714 );
PROVIDE ( strtoul = 0x4005692c );
PROVIDE ( _strtoul_r = 0x40056834 );
PROVIDE ( strupr = 0x4000174c );
PROVIDE ( __submore = 0x40058f3c );
PROVIDE ( _sungetc_r = 0x40057f6c );
PROVIDE ( __swbuf = 0x40058cb4 );
PROVIDE ( __swbuf_r = 0x40058bec );
PROVIDE ( __swrite = 0x40001150 );
PROVIDE ( __swsetup_r = 0x40058cc8 );
PROVIDE ( _system_r = 0x4000bc10 );
PROVIDE ( time = 0x40001844 );
PROVIDE ( __time_load_locale = 0x4000183c );
PROVIDE ( times = 0x40001808 );
PROVIDE ( toascii = 0x4000c720 );
PROVIDE ( tolower = 0x40001868 );
PROVIDE ( toupper = 0x40001884 );
PROVIDE ( __tzcalc_limits = 0x400018a0 );
PROVIDE ( __tz_lock = 0x40001a04 );
PROVIDE ( tzset = 0x40001a1c );
PROVIDE ( _tzset_r = 0x40001a28 );
PROVIDE ( __tz_unlock = 0x40001a10 );
PROVIDE ( ungetc = 0x400590f4 );
PROVIDE ( _ungetc_r = 0x40058fa0 );
PROVIDE ( utoa = 0x40056258 );
PROVIDE ( __utoa = 0x400561f0 );
PROVIDE ( wcrtomb = 0x40058920 );
PROVIDE ( _wcrtomb_r = 0x400588d8 );
PROVIDE ( _wctomb_r = 0x40058f14 );
PROVIDE ( write = 0x4000181c );

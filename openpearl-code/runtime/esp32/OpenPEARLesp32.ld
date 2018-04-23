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

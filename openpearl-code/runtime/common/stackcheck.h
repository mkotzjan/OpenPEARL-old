//IncludeCollector::PassThru begin
/**
\file stackcheck.h
*/
#ifndef NOSTACKCHECK

#if ((TARGET==2 && CONFIG_LPC1768_CHECK_STACK_OVERFLOW==1) || (TARGET==3 && CONFIG_ESP32_CHECK_STACK_OVERFLOW==1))
/**

Stack checking is useful but it costs lot of execution time for
templated functions.

There is an option to enable/disbale stack checking on the 
microcontroller part of the OpenPEARL system.

On normal linux systems this option is not supported by gcc.
*/
#define NOSTACKCHECK __attribute__((no_instrument_function))
#else
/**
\def NOSTACKCHECK

Stack checking is useful but it costs lot of execution time for
templated functions.

There is an option to enable/disbale stack checking on the 
microcontroller part of the OpenPEARL system.

On normal linux systems this option is not supoorted by gcc.
*/
#define NOSTACKCHECK /* nothing */
#endif
#endif
//IncludeCollector::PassThru end


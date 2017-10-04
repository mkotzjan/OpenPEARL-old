/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT file system module  R0.12b                             /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2016, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* add on for OpenPEARL:                                              */
/*   readable error texts                                             */
/*--------------------------------------------------------------------*/

#ifndef FF_ERRORS_INCLUDED
#define FF_ERRORS_INCLUDED
#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* f_strerror(FRESULT err);

#ifdef __cplusplus
}
#endif


#endif

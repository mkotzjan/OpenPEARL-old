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

#include "ff_errors.h"
#include "ff.h"
#include <stdio.h>

/* File function return code (FRESULT) */
static const char* error_messages[] = {
	"(0) Succeeded",
	"(1) A hard error occurred in the low level disk I/O layer",
	"(2) Assertion failed",
	"(3) The physical drive cannot work",
	"(4) Could not find the file",
	"(5) Could not find the path",
	"(6) The path name format is invalid",
	"(7) Access denied due to prohibited access or directory full",
	"(8) Access denied due to prohibited access",
	"(9) The file/directory object is invalid",
	"(10) The physical drive is write protected",
	"(11) The logical drive number is invalid",
	"(12) The volume has no work area",
	"(13) There is no valid FAT volume",
	"(14) The f_mkfs() aborted due to any problem",
	"(15) Could not get a grant to access the volume "
              "within defined period",
	"(16) The operation is rejected according to the file "
              "sharing policy",
	"(17) LFN working buffer could not be allocated",
	"(18) Number of open files > _FS_LOCK",
	"(19) Given parameter is invalid"};

const char* f_strerror(FRESULT err) {
    static char unknown_value[20];
    if (err >= 0 && err <=sizeof(error_messages)/sizeof(error_messages[0])) {
        return error_messages[err];
    }
    sprintf(unknown_value,"code=%d", err);
    return (unknown_value);
}



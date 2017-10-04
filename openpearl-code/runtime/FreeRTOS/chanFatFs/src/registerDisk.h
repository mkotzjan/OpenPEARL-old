/**
\file

*/

#ifndef _REGISTERDISK_DEFINED
#define _REGISTERDISK_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"


/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Map USB MSD to physical drive 2 */

typedef DSTATUS (*Disk_status)(BYTE pdrv);
typedef DSTATUS (*Disk_initialize)(BYTE pdrv);
typedef DRESULT (*Disk_read) ( BYTE pdrv,
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count);		/* Number of sectors to read */
typedef DRESULT (*Disk_write) ( BYTE pdrv,
	const BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count);		/* Number of sectors to read */
typedef DRESULT (*Disk_ioctl) (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff);		/* Buffer to send/receive control data */

struct DiskIoFunctions {
    Disk_status ds;
    Disk_initialize di;
    Disk_read dr;
    Disk_write dw;
    Disk_ioctl dctl;
};
/*---------------------------------------*/
/* Prototypes for disk control functions */

/**
register an i/o handler for the specified drive

\param pdrv identifier of the drive.
        Valid values are listed above as DEV_USB,..
\param diofs pointer to struct conatinig the function pointers to the handler 
             functions. 
             The struct which is passed by reference must have static linkage
\return 0, if ok\n
        != 0 , if error
*/
int disk_registerDrive(BYTE pdrv, struct DiskIoFunctions *diofs);


#ifdef __cplusplus
}
#endif

#endif

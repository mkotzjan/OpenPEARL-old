/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
/* modified for OpenPEARL with register method to attach a volume at     */
/* system startup							 */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

static struct DiskIoFunctions registered[3];

static const int nbrPdrv = sizeof(registered)/sizeof(registered[0]);

int disk_registerDrive(BYTE pdrv, struct DiskIoFunctions * diof) {
    if (pdrv < 0 || pdrv >= nbrPdrv) {
        return -1;
    }
    registered[pdrv] = *diof;
    return 0;  // ok
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv	/* Physical drive nmuber to identify the drive */
)
{
    int result;

    if (pdrv <0 || pdrv >= nbrPdrv || !registered[pdrv].ds) {
	return STA_NOINIT;
    }
    result = (*registered[pdrv].ds)(pdrv);
    return result;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv	/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS result;

    if (pdrv <0 || pdrv >= nbrPdrv || !registered[pdrv].di) {
	   return STA_NOINIT;
    }
    result = (*registered[pdrv].di)(pdrv);
    return result;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT result;

    if (pdrv <0 || pdrv >= nbrPdrv || !registered[pdrv].dr) {
	return RES_PARERR;
    }
    result = (*registered[pdrv].dr)(pdrv, buff, sector, count);
    return result;

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,	/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count		/* Number of sectors to write */
)
{
	DRESULT result;

    if (pdrv <0 || pdrv >= nbrPdrv || !registered[pdrv].dw) {
	return RES_PARERR;
    }
    result = (*registered[pdrv].dw)(pdrv, buff, sector, count);
    return result;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT result;

    if (pdrv <0 || pdrv >= nbrPdrv || !registered[pdrv].dctl) {
	return RES_PARERR;
    }
    result = (*registered[pdrv].dctl)(pdrv, cmd, buff);
    return result;

}


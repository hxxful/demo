/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	int result = 0;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_status();

		// translate the reslut code here
        stat = (DSTATUS)result;
		return stat;

	case DEV_MMC :
		//result = MMC_disk_status();
        result &= ~STA_NOINIT;
        
		// translate the reslut code here
        stat = (DSTATUS)result;
		return stat;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here
        stat = (DSTATUS)result;
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	int result = 0;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here
        stat = (DSTATUS)result;
		return stat;

	case DEV_MMC :
		//result = MMC_disk_initialize();
        result = EMMC_Init();
		// translate the reslut code here
        if(EMMC_OK == result) {
            stat &= ~STA_NOINIT; 
        }
		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here
        stat = (DSTATUS)result;
		return stat;
	}
	return STA_NOINIT;
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
	DRESULT res =  RES_PARERR;
	int result = 0;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here
        res = (DRESULT)result;
		return res;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);
        result = EMMC_ReadMultiBlocks(buff, sector*MyEMMC_CardInfo.CardBlockSize, MyEMMC_CardInfo.CardBlockSize, count);
		// translate the reslut code here
        if(EMMC_OK != result) {
            res = RES_ERROR;
            return res;
        }
        result = EMMC_WaitReadOperation();
        if(EMMC_OK != result) {
            res = RES_ERROR;
            return res;
        }
        while(EMMC_TRANSFER_OK != EMMC_GetStatus());
        res = RES_OK;
		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here
        res = (DRESULT)result;
		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_PARERR;
	int result = 0;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here
        res = (DRESULT)result;
		return res;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);
        DRV_EMMC_DEBUG("sec:0x%08X\r\n", sector);
        result = EMMC_WriteMultiBlocks((uint8_t *)buff, sector*MyEMMC_CardInfo.CardBlockSize, MyEMMC_CardInfo.CardBlockSize, count);
		// translate the reslut code here
        if(EMMC_OK != result) {
            res = RES_ERROR;
            return res;
        }
        result = EMMC_WaitWriteOperation();
        if(EMMC_OK != result) {
            res = RES_ERROR;
            return res;
        }
        while(EMMC_TRANSFER_OK != EMMC_GetStatus());
        res = RES_OK; 
		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here
        res = (DRESULT)result;
		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;
	int result = 0;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive
        res = (DRESULT)result;
		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card
		switch(cmd) {
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = MyEMMC_CardInfo.CardCapacity / MyEMMC_CardInfo.CardBlockSize;
                break;
            case GET_SECTOR_SIZE:
                *(DWORD*)buff = MyEMMC_CardInfo.CardBlockSize;
                break;
            case GET_BLOCK_SIZE:
                *(DWORD*)buff = 1;
                break;
                    
        }
        result = RES_OK;
        res = (DRESULT)result;
		return res;

	case DEV_USB :

		// Process of the command the USB drive
        res = (DRESULT)result;
		return res;
	}

	return RES_PARERR;
}

__weak DWORD get_fattime(void) {
#if 0
	/* 返回当前时间戳 */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
#else
    RTC_TimeTypeDef RTC_Time;
    RTC_DateTypeDef RTC_Date;
    RTC_GetTime(RTC_Format_BIN, &RTC_Time);
    RTC_GetDate(RTC_Format_BIN, &RTC_Date);

    return	  ((DWORD)(RTC_Date.RTC_Year + 2000 - 1980) << 25)	/* Year */
			| ((DWORD)RTC_Date.RTC_Month << 21)				/* Month */
			| ((DWORD)RTC_Date.RTC_Date << 16)				/* Mday */
			| ((DWORD)RTC_Time.RTC_Hours << 11)				/* Hour */
			| ((DWORD)RTC_Time.RTC_Minutes << 5)				  /* Min */
			| ((DWORD)RTC_Time.RTC_Seconds >> 1);				/* Sec */
#endif
}


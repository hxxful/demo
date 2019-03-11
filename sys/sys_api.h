#ifndef SYS_API_H
#define SYS_API_H

#include "os.h"
#include "app.h"
#include "drv_emmc.h"

//#define RAM_disk_initialize                 {return 0;}
//#define RAM_disk_status                     {return 0;}
//#define RAM_disk_read(a, b, c, d)           {return 0;}
//#define RAM_disk_write(a, b, c, d)          {return 0;}

#define MMC_disk_initialize                 EMMC_Init
#define MMC_disk_status                     EMMC_GetStatus
#define MMC_disk_read(a, b, c)              EMMC_ReadMultiBlocks(a, b*MyEMMC_CardInfo.CardBlockSize, MyEMMC_CardInfo.CardBlockSize, c)
#define MMC_disk_write(a, b, c)             EMMC_WriteMultiBlocks((uint8_t*)a, b*MyEMMC_CardInfo.CardBlockSize, MyEMMC_CardInfo.CardBlockSize, c)

//#define USB_disk_initialize                 {return 0;}
//#define USB_disk_status                     {return 0;}
//#define USB_disk_read(a, b, c, d)           {return 0;}
//#define USB_disk_write(a, b, c, d)          {return 0;}

#define SYS_ERR                             OS_ERR


#endif

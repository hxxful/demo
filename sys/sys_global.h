#ifndef SYS_GLOBAL_H
#define SYS_GLOBAL_H

/* 系统配置 */
#define OS_NONE
#define OS_UCOSIII

/* EMMC配置 */
#define EMMC_FS_FATFS
//#define EMMC_FS_EXT4
//#define EMMC_FS_DOS

#ifdef EMMC_FS_FATFS
#define EMMC_PATH "1:"
#define EMMC_SPEED_TEST_PATH    EMMC_PATH"ST"
#define EMMC_SPEED_TEST_FILE1   EMMC_SPEED_TEST_PATH"/tst1.bin"
#define EMMC_SPEED_TEST_FILE2   EMMC_SPEED_TEST_PATH"/tst2.bin"

#endif

/* DEBUG配置 */
#define APP_EMMC_DEBUG printf
#define DRV_EMMC_DEBUG printf

#define APP_P900_DEBUG
#define DRV_P900_DEBUG

#define APP_RTK_DEBUG
#define DRV_RTK_DEBUG


/* 系统变量 */
extern struct emmc emmc1;



#endif


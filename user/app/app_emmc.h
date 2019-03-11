#ifndef APP_EMMC_H
#define APP_EMMC_H

#include <stdint.h>
#include "sys_global.h"

#ifdef EMMC_FS_FATFS
#include "ff.h"

enum emmc_status {
    EMMC_STATUS_OK                      = 0,
    EMMC_STATUS_INIT_FAILED             = 1,
    EMMC_STATUS_MOUNT_FAILED            = 2,
    EMMC_STATUS_MKFS_FAILED             = 3,
    EMMC_STATUS_CREATE_DIR_FAILED       = 4,
    EMMC_STATUS_OPEN_FILE_FAILED        = 5,
    EMMC_STATUS_WRITE_FILE_FAILED       = 6
};

enum emmc_fs_type {
    EMMC_NOFS    = 0,
    EMMC_FATFS   = 1,
    EMMC_DOS     = 2,
    EMMC_EXT3    = 3
};

struct emmc_info {
    uint32_t    mode;
    uint32_t    size;
    uint32_t    write_speed;
    uint32_t    read_speed;
    uint32_t    fs;
};

struct emmc {
    enum emmc_status status;
    enum emmc_fs_type fs_type;    
    uint32_t write_speed;
    uint32_t read_speed;
    uint32_t capacity;
    uint32_t mode;
    uint32_t total_size;
    uint32_t free_size;
};

extern void emmc_mount(struct emmc* e);
extern void emmc_speed_test_read(struct emmc* e, const char* path);
extern void emmc_speed_test_write(struct emmc* e, uint8_t block_kb, uint32_t total_block_kb, const char* path);
extern void emmc_init(struct emmc* e);
extern struct emmc emmc1;



#endif /* EMMC_FS_FATFS*/

#ifdef EMMC_FS_DOS

#endif /* EMMC_FS_DOS */

#endif

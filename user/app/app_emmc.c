#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sys.h"
#include "sys_api.h"
#include "app_emmc.h"


#ifdef EMMC_FS_FATFS

FATFS my_fs;
FIL fd;
uint8_t work_buf[4096] = {0};
struct emmc emmc1;

void emmc_mkdir(struct emmc* e, const char* path)
{
    FRESULT res;
    if(EMMC_STATUS_OK == e->status) {
        res = f_mkdir(path);
        if(res != FR_OK && res != FR_EXIST) {
            e->status = EMMC_STATUS_CREATE_DIR_FAILED;
            APP_EMMC_DEBUG("emmc_mkdir: mkdir failed\r\n");
        }
    }
}


void emmc_speed_test_read(struct emmc* e, const char* path)
{
    static FIL fd;
    FRESULT res = FR_OK;
    int32_t ret = 0;
    uint32_t cn= 0;
    uint32_t br = 0;
    uint64_t t1 = 0;
    uint64_t t2 = 0;
    uint64_t dt_ms = 0;
    static uint8_t *buf = 0;
    static uint8_t *buf2 = 0;
    static uint32_t buf_size = 4096;
    static uint32_t buf2_size = 4096;
    static uint32_t rd_size = 0;
    static int8_t flag = 0;

    if(0 == flag) {
        res = f_open(&fd, path, FA_READ | FA_OPEN_EXISTING);
        if(FR_OK != res) {
            APP_EMMC_DEBUG("emmc_speed_test_read: open failed\r\n");
            return;
        }
        cn = f_size(&fd);
        if(0 == cn) {
            APP_EMMC_DEBUG("emmc_speed_test_read: file size 0\r\n");
            return;
        } else {
            APP_EMMC_DEBUG("emmc_speed_test_read: file size %dKB\r\n", cn / 1024);
            buf = (uint8_t *)malloc(buf_size + 50);
            buf2 = (uint8_t *)malloc(buf2_size + 50);
            if(NULL == buf || NULL == buf2) {
                APP_EMMC_DEBUG("emmc_speed_test_read: malloc error\r\n");
                return;
            }
            memset((void *)buf, 0x00, buf_size);
            memset((void *)buf2, 0xAA, buf2_size);
            flag = 1;
        }
    }
    
    if(1 == flag) {
        memset(buf, 0x00, buf_size);
        t1 = sys_time_get_ms();
        res = f_read(&fd, buf, buf_size, &br);
        if(FR_OK != res) {
            APP_EMMC_DEBUG("emmc_speed_test_read: read failed\r\n");
            res = f_close(&fd);
            if(FR_OK != res) {
                APP_EMMC_DEBUG("emmc_speed_test_read: close failed\r\n");
                return;
            }
            free(buf);
            free(buf2);
            buf = 0;
            buf2 = 0;
            flag = 0;
            return;
        }
        t2 = sys_time_get_ms();
        dt_ms = t2 - t1;
        rd_size++;
        ret = memcmp(buf, buf2, br);
        if(0 != ret) {
            APP_EMMC_DEBUG("memcmp error\r\n");
            return;
        }
        if(0 == br) {
            flag = 2;
        }
        APP_EMMC_DEBUG("%lld %d %d\r\n", dt_ms, br, rd_size);
    }
    if(2 == flag) {
        res = f_close(&fd);
        if(FR_OK != res) {
            APP_EMMC_DEBUG("emmc_speed_test_read: close failed\r\n");
            return;
        } 
    }
}

/* eMMC 写入时间测试 */
void emmc_speed_test_write(struct emmc* e, uint8_t block_kb, uint32_t total_block_kb, const char* path)
{
    FRESULT res;
    //FIL fd;
    static uint8_t* buf = NULL;
    uint32_t bw = 0;
    uint64_t t1 = 0;
    uint64_t t2 = 0;
    uint64_t t3 = 0;
    uint64_t t4 = 0;
    static uint32_t et = 0;
    static uint64_t ck = 0;
    static uint64_t dt_us = 0;
    static uint64_t dt_ms = 0;
    static uint8_t flag = 0;
   
    if(0 == flag) {
        if(EMMC_STATUS_OK == e->status) {
            res = f_mkdir(EMMC_SPEED_TEST_PATH);
            if(res != FR_OK && res != FR_EXIST) {
                e->status = EMMC_STATUS_CREATE_DIR_FAILED;
                APP_EMMC_DEBUG("emmc_init: mkdir failed: %d\r\n", res);
                return;
            }
            flag = 1;
        }
    } 
    
    if(1 == flag) {
        res = f_unlink(path);
        if(FR_OK != res && FR_NO_FILE != res) {
            APP_EMMC_DEBUG("emmc_speed_test_write: delet %s file failed\r\n", path);
        }
        APP_EMMC_DEBUG("emmc_speed_test_write: delet %s file ok\r\n", path);
        res = f_open(&fd, path, FA_WRITE | FA_OPEN_ALWAYS);
        if(FR_OK != res) {
            APP_EMMC_DEBUG("emmc_speed_test_write: open failed\r\n");
            return;
        }
        buf = (uint8_t*)malloc(block_kb * 1024);
        memset(buf, 0xAA, block_kb * 1024);
        flag = 2;
    }
    
    if(2 == flag) {
        t1 = sys_time_get_ms();
        t3 = sys_time_get_us();
        res = f_write(&fd, buf, block_kb * 1024, &bw);        
        if(FR_OK != res) {
            et++;
            APP_EMMC_DEBUG("emmc_speed_test_write: write failed\r\n");
            return;
        }
        res = f_sync(&fd);
        if(FR_OK != res) {
            APP_EMMC_DEBUG("emmc_speed_test_write: sync failed\r\n");
            return;
        }
        t4 = sys_time_get_us(); 
        t2 = sys_time_get_ms();
        dt_ms = (t2 - t1);
        dt_us = (t4 - t3);
        ck++;
        //APP_EMMC_DEBUG("%lldms %lldus %lld\r\n", dt_ms, dt_us, ck);
        //APP_EMMC_DEBUG("%lld  %lld\r\n", dt_ms, dt_us);
        APP_EMMC_DEBUG("%lld\r\n", dt_ms);
        if(ck >= total_block_kb) {                    
            free(buf);
            buf = NULL;
            flag = 3;
        }
    }
    
    if(3 == flag) {
        res = f_close(&fd);
        if(FR_OK != res) {
            APP_EMMC_DEBUG("emmc_speed_test_write: close failed\r\n");
            return;
        } else {
            APP_EMMC_DEBUG("emmc_speed_test_write: test file write ok\r\n");
        }
        flag = 4;
    }   
}

void emmc_info(struct emmc* e)
{
    FRESULT res;
    FATFS *pfs;
    DWORD free_clust;
    pfs = &my_fs;
    res = f_getfree(EMMC_PATH, &free_clust, &pfs);
    if(FR_OK != res) {
        APP_EMMC_DEBUG("emmc info error:%d\r\n", res);
    } else {
        e->total_size = (pfs->n_fatent - 2) * pfs->csize / 2;
        e->free_size = free_clust * pfs->csize / 2;
    }
    //APP_EMMC_DEBUG("emmc info: size %d free %d\r\n", e->total_size, e->free_size);
    APP_EMMC_DEBUG("emmc info: total size %0.1fM free %0.1fM\r\n", (float)(e->total_size) / 1024, (float)(e->free_size) / 1024);
    //APP_EMMC_DEBUG("emmc info: total size %0.1fG free %0.1fG\r\n", (float)(e->total_size) / 1024 / 1024, (float)(e->free_size) / 1024 / 1024);
}

void emmc_mount(struct emmc* e)
{
    FRESULT res = FR_OK;
    e->status = EMMC_STATUS_OK;
    
    /* 挂载文件系统 */
    res = f_mount(&my_fs, EMMC_PATH, 1);
    APP_EMMC_DEBUG("emmc first mount: %d\r\n", res);
    //res = FR_NO_FILESYSTEM;
    if(FR_NO_FILESYSTEM == res) {
        /* 格式化文件系统 */
        //res = f_mkfs(EMMC_PATH, FM_FAT32, 0, work_buf, 2048);
        res = f_mkfs(EMMC_PATH, 0, 0);
        if(FR_OK == res) {
            /* 取消挂载 */
            res = f_mount(NULL, EMMC_PATH, 1);
            /* 重新挂载 */
            res = f_mount(&my_fs, EMMC_PATH, 1);
            if(FR_OK == res) {
                APP_EMMC_DEBUG("emmc mount ok\r\n");
            }
        } else {
            e->status = EMMC_STATUS_MKFS_FAILED;
            APP_EMMC_DEBUG("emmc format failed\r\n");
        }
    } else if(FR_OK != res) {
        e->status = EMMC_STATUS_MOUNT_FAILED;
        APP_EMMC_DEBUG("emmc mount failed %d\r\n", res);
    } else {
        e->fs_type = EMMC_FATFS;
        APP_EMMC_DEBUG("emmc mount ok\r\n");
    }    
}

void emmc_init(struct emmc* e)
{ 
    EMMC_Error res = EMMC_OK;
//    APP_EMMC_DEBUG("\r\n**********EMMC Init************\r\n");
//    sys_delay_ms(1000);
//    APP_EMMC_DEBUG("Starting eMMC Init\r\n");
//    res = EMMC_Init();
//    if(res != EMMC_OK) {
//        e->status = EMMC_STATUS_INIT_FAILED;
//        return;
//    }
    
    emmc_mount(e);
    if(EMMC_STATUS_OK == e->status)
        emmc_info(e);

    uint8_t buf[512] = {0};
    uint8_t buf2[512] = {0};
    memset(buf, 0xAA, 512);
    memset(buf2, 0x00, 512);
    
    for(uint32_t i = 0; i < 1000; i++) {
        res = EMMC_WriteMultiBlocks(buf, 0x5F+i, 0x200, 1);
        if(res != EMMC_OK) {
            APP_EMMC_DEBUG("EMMC WriteMultiBlock Error:%d\r\n", res);
        }
        APP_EMMC_DEBUG("EMMC WriteMultiBlock OK\r\n");
    }
    
    for(uint32_t i = 0; i < 1000; i++) {
        memset(buf2, 0x00, 512);
        res = EMMC_ReadMultiBlocks(buf2, 0x5F+i, 0x200, 1);
        if(res != EMMC_OK) {
            APP_EMMC_DEBUG("EMMC ReadMultiBlock Error:%d\r\n", res);
        }
        if(memcmp(buf, buf2, 512) != 0)
            APP_EMMC_DEBUG("EMMC ReadMultiBlock Error:%d\r\n", res);
        APP_EMMC_DEBUG("EMMC ReadMultiBlock OK\r\n");
    }
    
    
}


#endif /* EMMC_FS_FTAFS */

#ifdef EMMC_FS_EXT4
#endif /* EMMC_FS_EXT4*/


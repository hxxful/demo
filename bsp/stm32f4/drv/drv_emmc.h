/**
  ******************************************************************************
  * @file    drv_emmc.h
  * @author  HouXianxin 
  * @version V1.0.0
  * @date    2019/02/13
  * @brief   This file is Logic file.
  ******************************************************************************
	**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMMC_SDIO_H
#define __EMMC_SDIO_H


#define USE_EMMC


#ifdef __cplusplus
 extern "C" {
#endif
 
#include <stm32f4xx.h>



     
//#include "define.h"
/** @defgroup SDIO_EMMC_Exported_Types
  * @{
  */

	 
/** 
  * @brief  SDIO specific error defines  
  */   
typedef enum
{
 
  EMMC_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
  EMMC_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
  EMMC_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
  EMMC_DATA_TIMEOUT                    = (4), /*!< Data time out */
  EMMC_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
  EMMC_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
  EMMC_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
  EMMC_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
  EMMC_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
  EMMC_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  EMMC_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
  EMMC_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
  EMMC_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
  EMMC_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  EMMC_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
  EMMC_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
  EMMC_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
  EMMC_CC_ERROR                        = (18), /*!< Internal card controller error */
  EMMC_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
  EMMC_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
  EMMC_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
  EMMC_CID_CEMMC_OVERWRITE             = (22), /*!< CID/CSD overwrite error */
  EMMC_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
  EMMC_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
  EMMC_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
  EMMC_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
  EMMC_INVALID_VOLTRANGE               = (27),
  EMMC_ADDR_OUT_OF_RANGE               = (28),
  EMMC_SWITCH_ERROR                    = (29),
  EMMC_SDIO_DISABLED                   = (30),
  EMMC_SDIO_FUNCTION_BUSY              = (31),
  EMMC_SDIO_FUNCTION_FAILED            = (32),
  EMMC_SDIO_UNKNOWN_FUNCTION           = (33),

/** 
  * @brief  Standard error defines   
  */ 
  EMMC_INTERNAL_ERROR, 
  EMMC_NOT_CONFIGURED,
  EMMC_REQUEST_PENDING, 
  EMMC_REQUEST_NOT_APPLICABLE, 
  EMMC_INVALID_PARAMETER,  
  EMMC_UNSUPPORTED_FEATURE,  
  EMMC_UNSUPPORTED_HW,  
  EMMC_ERROR,  
  EMMC_OK = 0 
} EMMC_Error;

/** 
  * @brief  SDIO Transfer state  
  */   
typedef enum
{
  EMMC_TRANSFER_OK  = 0,
  EMMC_TRANSFER_BUSY = 1,
  EMMC_TRANSFER_ERROR
} EMMCTransferState;

/** 
  * @brief  SD Card States 
  */   
typedef enum
{
  EMMC_CARD_READY                  = ((uint32_t)0x00000001),
  EMMC_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
  EMMC_CARD_STANDBY                = ((uint32_t)0x00000003),
  EMMC_CARD_TRANSFER               = ((uint32_t)0x00000004),
  EMMC_CARD_SENDING                = ((uint32_t)0x00000005),
  EMMC_CARD_RECEIVING              = ((uint32_t)0x00000006),
  EMMC_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
  EMMC_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
  EMMC_CARD_ERROR                  = ((uint32_t)0x000000FF)
}EMMCCardState;


/** 
  * @brief  Card Specific Data: CSD Register   
  */ 
typedef struct
{
  __IO uint8_t  CSD_STRUCTURE;              /*!< Card-Specific Data structure */
  __IO uint8_t  SPEC_VERS;                  /*!< System specification version */
  __IO uint8_t  Reserved1;                  /*!< Reserved */
  __IO uint8_t  TAAC;                       /*!< Data read access-time 1 */
  __IO uint8_t  NSAC;                       /*!< Data read access-time 2 in CLK cycles */
  __IO uint8_t  TRAN_SPEED;                 /*!< Max. bus clock frequency */
  __IO uint16_t CCC;                        /*!< Card command classes */
  __IO uint8_t  READ_BL_LEN;                /*!< Max. read data block length */
  __IO uint8_t  READ_BL_PARTIAL;            /*!< Partial blocks for read allowed */
  __IO uint8_t  WRITE_BLK_MISALIGN;         /*!< Write block misalignment */
  __IO uint8_t  READ_BLK_MISALIGN;          /*!< Read block misalignment */
  __IO uint8_t  DSR_IMP;                    /*!< DSR implemented */
  __IO uint8_t  Reserved2;                  /*!< Reserved */
  __IO uint16_t C_SIZE;                     /*!< Device Size */
  __IO uint8_t  VDD_R_CURR_MIN;             /*!< Max. read current @ VDD min */
  __IO uint8_t  VDD_R_CURR_MAX;             /*!< Max. read current @ VDD max */
  __IO uint8_t  VDD_W_CURR_MIN;             /*!< Max. write current @ VDD min */
  __IO uint8_t  VDD_W_CURR_MAX;             /*!< Max. write current @ VDD max */
  __IO uint8_t  C_SIZE_MULT;                /*!< Device size multiplier */
  __IO uint8_t  ERASE_GRP_SIZE;             /*!< Erase group size */
  __IO uint8_t  ERASE_GRP_MULT;             /*!< Erase group size multiplier */
  __IO uint8_t  WP_GRP_SIZE;                /*!< Write protect group size */
  __IO uint8_t  WP_GRP_ENABLE;              /*!< Write protect group enable */
  __IO uint8_t  DEFAULT_ECC;                /*!< Manufacturer default ECC */
  __IO uint8_t  R2W_FACTOR;                 /*!< Write speed factor */
  __IO uint8_t  WRITE_BL_LEN;               /*!< Max. write data block length */
  __IO uint8_t  WRITE_BL_PARTIAL;           /*!< Partial blocks for write allowed */
  __IO uint8_t  Reserved3;                  /*!< Reserded */
  __IO uint8_t  CONTENT_PROT_APP;           /*!< Content protection application */
  __IO uint8_t  FILE_FORMAT_GRP;            /*!< File format group */
  __IO uint8_t  COPY;                       /*!< Copy flag (OTP) */
  __IO uint8_t  PERM_WRITE_PROTECT;         /*!< Permanent write protection */
  __IO uint8_t  TMP_WRITE_PROTECT;          /*!< Temporary write protection */
  __IO uint8_t  FILE_FORMAT;                /*!< File Format */
  __IO uint8_t  ECC;                        /*!< ECC code */
  __IO uint8_t  CSD_CRC;                    /*!< CSD CRC */
  __IO uint8_t  Reserved4;                  /*!< always 1*/
} EMMC_CSD;


#define KLM8G1GEAC_B001
//#define KLM8G1GETF_B041

/** 
  * @brief  Card Specific Data: EXTCSD Register, Total Size 512 bytes  
  */ 
#ifdef KLM8G1GEAC_B001
typedef union
{
		struct _EXT_CSD
		{
		    /*******************************Mode Segment********************************/
			__IO uint8_t    Reserved1[32];								  /* [0-31]    */
			__IO uint8_t 	FLUSH_CACHE;	                        	  /* [32] 	   */
			__IO uint8_t 	CACHE_CTRL; 	                        	  /* [33] 	   */
			__IO uint8_t 	POWER_OFF_NOTIFICATION;	                	  /* [34] 	   */
			__IO uint8_t 	PACKED_FAILURE_INDEX;	                	  /* [35] 	   */
			__IO uint8_t 	PACKED_COMMAND_STATUS;	                	  /* [36] 	   */
			__IO uint8_t 	CONTEXT_CONF[15]; 	                    	  /* [37-51]   */
			__IO uint8_t 	EXT_PARTITIONS_ATTRIBUTE[2];	        	  /* [52-53]   */
			__IO uint8_t 	EXCEPTION_EVENTS_STATUS[2];	            	  /* [54-55]   */
			__IO uint8_t 	EXCEPTION_EVENTS_CTRL[2];	            	  /* [56-57]   */
			__IO uint8_t 	DYNCAP_NEEDED; 	                        	  /* [58] 	   */
			__IO uint8_t 	CLASS_6_CTRL; 	                        	  /* [59] 	   */
			__IO uint8_t 	INI_TIMEOUT_EMU; 	                    	  /* [60] 	   */
			__IO uint8_t 	DATA_SECTOR_SIZE; 	                    	  /* [61] 	   */
			__IO uint8_t 	USE_NATIVE_SECTOR; 	                    	  /* [62] 	   */
			__IO uint8_t 	NATIVE_SECTOR_SIZE;	                    	  /* [63] 	   */
            __IO uint8_t 	OPTIMIZED_FEATURES[2];	                	  /* [64-65]   */
            __IO uint8_t    Reserved2;		                        	  /* [66]      */
            __IO uint8_t    AUTO_BKOP_CONFIG;		                      /* [67]      */
            __IO uint8_t    HIGH_PERF_CONFIG;		                	  /* [68]      */
			__IO uint8_t    Reserved3[61];		                    	  /* [69-129]  */
			__IO uint8_t 	PROGRAM_CID_CSD_DDR_SUPPORT;	        	  /* [130] 	   */
			__IO uint8_t 	PERIODIC_WAKEUP; 	                    	  /* [131] 	   */
			__IO uint8_t 	TCASE_SUPPORT; 	                        	  /* [132] 	   */
			__IO uint8_t    Reserved4;		        	                  /* [133] 	   */
			__IO uint8_t 	SEC_BAD_BLK_MGMNT;	                    	  /* [134] 	   */
			__IO uint8_t    Reserved5;		                        	  /* [135] 	   */
			__IO uint8_t 	ENH_START_ADDR[4];	                    	  /* [136-139] */
			__IO uint8_t 	ENH_SIZE_MULT[3]; 	                    	  /* [140-142] */
			__IO uint8_t 	GP_SIZE_MULT[12]; 	                    	  /* [143-154] */
			__IO uint8_t 	PARTITION_SETTING_COMPLETED;	        	  /* [155] 	   */
			__IO uint8_t 	PARTITIONS_ATTRIBUTE; 	                	  /* [156] 	   */
			__IO uint8_t 	MAX_ENH_SIZE_MULT[3]; 	                	  /* [157-159] */
			__IO uint8_t 	PARTITIONING_SUPPORT; 	                	  /* [160] 	   */
			__IO uint8_t 	HPI_MGMT; 	                            	  /* [161] 	   */
			__IO uint8_t 	RST_n_FUNCTION; 	                    	  /* [162] 	   */
			__IO uint8_t 	BKOPS_EN; 	                            	  /* [163] 	   */
			__IO uint8_t 	BKOPS_START; 	                        	  /* [164] 	   */
			__IO uint8_t 	SANITIZE_START; 	                    	  /* [165] 	   */
			__IO uint8_t 	WR_REL_PARAM; 	                        	  /* [166] 	   */
			__IO uint8_t 	WR_REL_SET; 	                        	  /* [167] 	   */
			__IO uint8_t 	RPMB_SIZE_MULT; 	                    	  /* [168] 	   */
			__IO uint8_t 	FW_CONFIG; 	                            	  /* [169] 	   */
			__IO uint8_t    Reserved6;		                        	  /* [170] 	   */
			__IO uint8_t 	USER_WP;	                            	  /* [171] 	   */
			__IO uint8_t    Reserve7;		                        	  /* [172] 	   */
			__IO uint8_t 	BOOT_WP;	                            	  /* [173] 	   */
			__IO uint8_t 	BOOT_WP_STATUS; 	                    	  /* [174] 	   */
			__IO uint8_t 	ERASE_GROUP_DEF; 	                    	  /* [175] 	   */
			__IO uint8_t    Reserved8;		                        	  /* [176] 	   */
			__IO uint8_t    BOOT_BUS_CONDITIONS;		            	  /* [177] 	   */
			__IO uint8_t    BOOT_CONFIG_PROT; 		                	  /* [178] 	   */
			__IO uint8_t 	PARTITION_CONFIG; 	                    	  /* [179] 	   */
			__IO uint8_t    Reserved9;		                        	  /* [180] 	   */
			__IO uint8_t    ERASED_MEM_CONT;		                	  /* [181] 	   */
			__IO uint8_t    Reserved10;		                        	  /* [182] 	   */
			__IO uint8_t    BUS_WIDTH;		                        	  /* [183] 	   */
			__IO uint8_t    Reserved11;		                    	      /* [184] 	   */
			__IO uint8_t    HS_TIMING;		                        	  /* [185] 	   */
			__IO uint8_t    Reserved12;		                        	  /* [186] 	   */
			__IO uint8_t    POWER_CLASS;		                    	  /* [187] 	   */
			__IO uint8_t    Reserved13;		                        	  /* [188] 	   */
			__IO uint8_t    CMD_SET_REV;		                    	  /* [189] 	   */
			__IO uint8_t    Reserved14;		                        	  /* [190] 	   */
			__IO uint8_t    CMD_SET;		                        	  /* [191] 	   */
            /*******************************Properties Segment**************************/
			__IO uint8_t    EXT_CSD_REV;		                    	  /* [192] 	   */
			__IO uint8_t    Reserved15 ;		                    	  /* [193] 	   */
			__IO uint8_t    CSD_STRUCTURE;		                    	  /* [194] 	   */
			__IO uint8_t    Reserved17;		                        	  /* [195] 	   */
			__IO uint8_t    DEVICE_TYPE;		                    	  /* [196] 	   */
			__IO uint8_t    DRIVER_STRENGTH; 		                	  /* [197] 	   */
			__IO uint8_t    OUT_OF_INTERRUPT_TIME;		            	  /* [198] 	   */
			__IO uint8_t    PARTITION_SWITCH_TIME;		            	  /* [199] 	   */
			__IO uint8_t    PWR_CL_52_195; 		                    	  /* [200] 	   */
			__IO uint8_t    PWR_CL_26_195; 		                    	  /* [201] 	   */
			__IO uint8_t    PWR_CL_52_360; 		                    	  /* [202] 	   */
			__IO uint8_t    PWR_CL_26_360; 		                    	  /* [203] 	   */
			__IO uint8_t    Reserved19;		                        	  /* [204] 	   */
			__IO uint8_t    MIN_PERF_R_4_26;		                	  /* [205] 	   */
			__IO uint8_t    MIN_PERF_W_4_26; 		                	  /* [206] 	   */
			__IO uint8_t    MIN_PERF_R_8_26_4_52; 		            	  /* [207] 	   */
			__IO uint8_t    MIN_PERF_W_8_26_4_52; 		            	  /* [208] 	   */
			__IO uint8_t    MIN_PERF_R_8_52; 		                	  /* [209] 	   */
			__IO uint8_t    MIN_PERF_W_8_52; 		                	  /* [210] 	   */
			__IO uint8_t    Reserved20;		                    	      /* [211] 	   */
			__IO uint8_t    SEC_COUNT[4];		                    	  /* [212-215] */
			__IO uint8_t    SLEEP_NOTIFICATION_TIME;		        	  /* [216] 	   */
			__IO uint8_t    S_A_TIMEOUT;		                    	  /* [217] 	   */
			__IO uint8_t    Reserved21;     	                          /* [218] 	   */
			__IO uint8_t    S_C_VCCQ; 		                        	  /* [219] 	   */
			__IO uint8_t    S_C_VCC; 		                        	  /* [220] 	   */
			__IO uint8_t    HC_WP_GRP_SIZE; 		                	  /* [221] 	   */
			__IO uint8_t    REL_WR_SEC_C; 		                    	  /* [222] 	   */
			__IO uint8_t    ERASE_TIMEOUT_MULT; 		            	  /* [223] 	   */
			__IO uint8_t    HC_ERASE_GRP_SIZE; 		                	  /* [224] 	   */
			__IO uint8_t    ACC_SIZE; 		                        	  /* [225] 	   */
			__IO uint8_t    BOOT_SIZE_MULTI; 		                	  /* [226] 	   */
			__IO uint8_t    Reserved22;		                        	  /* [227] 	   */
			__IO uint8_t    BOOT_INFO; 		                        	  /* [228] 	   */
			__IO uint8_t    SEC_TRIM_MULT;		                    	  /* [229] 	   */
			__IO uint8_t    SEC_ERASE_MULT;		                    	  /* [230] 	   */
			__IO uint8_t    SEC_FEATURE_SUPPORT; 		            	  /* [231] 	   */
			__IO uint8_t    TRIM_MULT; 		                        	  /* [232] 	   */
			__IO uint8_t    Reserved23;		                        	  /* [233] 	   */
			__IO uint8_t    MIN_PERF_DDR_R_8_52;		            	  /* [234] 	   */
			__IO uint8_t    MIN_PERF_DDR_W_8_52; 		            	  /* [235] 	   */
			__IO uint8_t    PWR_CL_200_195; 		                	  /* [236] 	   */
			__IO uint8_t    PWR_CL_200_360; 		                	  /* [237] 	   */
			__IO uint8_t    PWR_CL_DDR_52_195; 		                	  /* [238] 	   */
			__IO uint8_t    PWR_CL_DDR_52_360; 		                	  /* [239] 	   */
			__IO uint8_t    Reserved24;		                        	  /* [240] 	   */
			__IO uint8_t    INI_TIMEOUT_AP;		                    	  /* [241] 	   */
			__IO uint8_t    CORRECTLY_PRG_SECTORS_NUM[4];		    	  /* [242-245] */
			__IO uint8_t    BKOPS_STATUS;		                    	  /* [246]     */
			__IO uint8_t    POWER_OFF_LONG_TIME;		            	  /* [247] 	   */
			__IO uint8_t    GENERIC_CMD6_TIME; 		                	  /* [248] 	   */
			__IO uint8_t    CACHE_SIZE[4]; 		                    	  /* [249-252] */
            __IO uint8_t    Reserved25[241];		                      /* [253-493] */
			__IO uint8_t    EXT_SUPPORT;		                    	  /* [494]	   */
			__IO uint8_t    LARGE_UNIT_SIZE_M1; 		            	  /* [495]	   */
			__IO uint8_t    CONTEXT_CAPABILITIES; 		            	  /* [496]	   */
			__IO uint8_t    TAG_RES_SIZE; 		                    	  /* [497]	   */
			__IO uint8_t    TAG_UNIT_SIZE; 		                    	  /* [498]	   */
			__IO uint8_t    DATA_TAG_SUPPORT; 		                	  /* [499]	   */
			__IO uint8_t    MAX_PACKED_WRITES; 		                	  /* [500]	   */
			__IO uint8_t    MAX_PACKED_READS; 		                	  /* [501]	   */
			__IO uint8_t    BKOPS_SUPPORT;		                    	  /* [502]	   */
			__IO uint8_t    HPI_FEATURES; 		                    	  /* [503]	   */
			__IO uint8_t    S_CMD_SET;		                        	  /* [504]	   */
			__IO uint8_t    Reserved26[7];	                        	  /* [505-511] */
		} EXT_CSD;
   __IO uint8_t CsdBuf[512];
} EMMCEXT_CSD;

#endif

#ifdef KLM8G1GETF_B041
typedef union
{
		struct _EXT_CSD
		{
		    /*******************************Mode Segment********************************/
			__IO uint8_t    Reserved1[15];								  /* [0-14]    */
            __IO uint8_t    CMDQ_MODE_EN;                           	  /* [15]      */
            __IO uint8_t    SECURE_REMOVAL_TYPE;                    	  /* [16]      */
            __IO uint8_t    PRODUCT_STATE_AWARENESS_ENABLEMENT;     	  /* [17]      */
            __IO uint8_t    MAX_PRE_LOADING_DATA_SIZE[4];           	  /* [18-21]   */
            __IO uint8_t    PRE_LOADING_DATA_SIZE[4];               	  /* [22-25]   */
            __IO uint8_t    FFU_STATUS;                             	  /* [26]      */
            __IO uint8_t    Reserved2[2];                           	  /* [27-28]   */
            __IO uint8_t    MODE_OPERATION_CODES;                   	  /* [29] 	   */
            __IO uint8_t    MODE_CONFIG;                            	  /* [30] 	   */
            __IO uint8_t    BARRIER_CTRL;                           	  /* [31] 	   */
			__IO uint8_t 	FLUSH_CACHE;	                        	  /* [32] 	   */
			__IO uint8_t 	CACHE_CTRL; 	                        	  /* [33] 	   */
			__IO uint8_t 	POWER_OFF_NOTIFICATION;	                	  /* [34] 	   */
			__IO uint8_t 	PACKED_FAILURE_INDEX;	                	  /* [35] 	   */
			__IO uint8_t 	PACKED_COMMAND_STATUS;	                	  /* [36] 	   */
			__IO uint8_t 	CONTEXT_CONF[15]; 	                    	  /* [37-51]   */
			__IO uint8_t 	EXT_PARTITIONS_ATTRIBUTE[2];	        	  /* [52-53]   */
			__IO uint8_t 	EXCEPTION_EVENTS_STATUS[2];	            	  /* [54-55]   */
			__IO uint8_t 	EXCEPTION_EVENTS_CTRL[2];	            	  /* [56-57]   */
			__IO uint8_t 	DYNCAP_NEEDED; 	                        	  /* [58] 	   */
			__IO uint8_t 	CLASS_6_CTRL; 	                        	  /* [59] 	   */
			__IO uint8_t 	INI_TIMEOUT_EMU; 	                    	  /* [60] 	   */
			__IO uint8_t 	DATA_SECTOR_SIZE; 	                    	  /* [61] 	   */
			__IO uint8_t 	USE_NATIVE_SECTOR; 	                    	  /* [62] 	   */
			__IO uint8_t 	NATIVE_SECTOR_SIZE;	                    	  /* [63] 	   */
			__IO uint8_t 	VENDOR_SPECIFIC_FIELD[64];	            	  /* [64-127]  */
			__IO uint8_t    Reserved3[2];		                    	  /* [128-129] */
			__IO uint8_t 	PROGRAM_CID_CSD_DDR_SUPPORT;	        	  /* [130] 	   */
			__IO uint8_t 	PERIODIC_WAKEUP; 	                    	  /* [131] 	   */
			__IO uint8_t 	TCASE_SUPPORT; 	                        	  /* [132] 	   */
			__IO uint8_t    PRODUCTION_STATE_AWARENESS;		        	  /* [133] 	   */
			__IO uint8_t 	SEC_BAD_BLK_MGMNT;	                    	  /* [134] 	   */
			__IO uint8_t    Reserved5;		                        	  /* [135] 	   */
			__IO uint8_t 	ENH_START_ADDR[4];	                    	  /* [136-139] */
			__IO uint8_t 	ENH_SIZE_MULT[3]; 	                    	  /* [140-142] */
			__IO uint8_t 	GP_SIZE_MULT[12]; 	                    	  /* [143-154] */
			__IO uint8_t 	PARTITION_SETTING_COMPLETED;	        	  /* [155] 	   */
			__IO uint8_t 	PARTITIONS_ATTRIBUTE; 	                	  /* [156] 	   */
			__IO uint8_t 	MAX_ENH_SIZE_MULT[3]; 	                	  /* [157-159] */
			__IO uint8_t 	PARTITIONING_SUPPORT; 	                	  /* [160] 	   */
			__IO uint8_t 	HPI_MGMT; 	                            	  /* [161] 	   */
			__IO uint8_t 	RST_n_FUNCTION; 	                    	  /* [162] 	   */
			__IO uint8_t 	BKOPS_EN; 	                            	  /* [163] 	   */
			__IO uint8_t 	BKOPS_START; 	                        	  /* [164] 	   */
			__IO uint8_t 	SANITIZE_START; 	                    	  /* [165] 	   */
			__IO uint8_t 	WR_REL_PARAM; 	                        	  /* [166] 	   */
			__IO uint8_t 	WR_REL_SET; 	                        	  /* [167] 	   */
			__IO uint8_t 	RPMB_SIZE_MULT; 	                    	  /* [168] 	   */
			__IO uint8_t 	FW_CONFIG; 	                            	  /* [169] 	   */
			__IO uint8_t    Reserved6;		                        	  /* [170] 	   */
			__IO uint8_t 	USER_WP;	                            	  /* [171] 	   */
			__IO uint8_t    Reserve7;		                        	  /* [172] 	   */
			__IO uint8_t 	BOOT_WP;	                            	  /* [173] 	   */
			__IO uint8_t 	BOOT_WP_STATUS; 	                    	  /* [174] 	   */
			__IO uint8_t 	ERASE_GROUP_DEF; 	                    	  /* [175] 	   */
			__IO uint8_t    Reserved8;		                        	  /* [176] 	   */
			__IO uint8_t    BOOT_BUS_CONDITIONS;		            	  /* [177] 	   */
			__IO uint8_t    BOOT_CONFIG_PROT; 		                	  /* [178] 	   */
			__IO uint8_t 	PARTITION_CONFIG; 	                    	  /* [179] 	   */
			__IO uint8_t    Reserved9;		                        	  /* [180] 	   */
			__IO uint8_t    ERASED_MEM_CONT;		                	  /* [181] 	   */
			__IO uint8_t    Reserved10;		                        	  /* [182] 	   */
			__IO uint8_t    BUS_WIDTH;		                        	  /* [183] 	   */
			__IO uint8_t    STROBE_SUPPORT;		                    	  /* [184] 	   */
			__IO uint8_t    HS_TIMING;		                        	  /* [185] 	   */
			__IO uint8_t    Reserved12;		                        	  /* [186] 	   */
			__IO uint8_t    POWER_CLASS;		                    	  /* [187] 	   */
			__IO uint8_t    Reserved13;		                        	  /* [188] 	   */
			__IO uint8_t    CMD_SET_REV;		                    	  /* [189] 	   */
			__IO uint8_t    Reserved14;		                        	  /* [190] 	   */
			__IO uint8_t    CMD_SET;		                        	  /* [191] 	   */
            /*******************************Properties Segment**************************/
			__IO uint8_t    EXT_CSD_REV;		                    	  /* [192] 	   */
			__IO uint8_t    Reserved15 ;		                    	  /* [193] 	   */
			__IO uint8_t    CSD_STRUCTURE;		                    	  /* [194] 	   */
			__IO uint8_t    Reserved17;		                        	  /* [195] 	   */
			__IO uint8_t    DEVICE_TYPE;		                    	  /* [196] 	   */
			__IO uint8_t    DRIVER_STRENGTH; 		                	  /* [197] 	   */
			__IO uint8_t    OUT_OF_INTERRUPT_TIME;		            	  /* [198] 	   */
			__IO uint8_t    PARTITION_SWITCH_TIME;		            	  /* [199] 	   */
			__IO uint8_t    PWR_CL_52_195; 		                    	  /* [200] 	   */
			__IO uint8_t    PWR_CL_26_195; 		                    	  /* [201] 	   */
			__IO uint8_t    PWR_CL_52_360; 		                    	  /* [202] 	   */
			__IO uint8_t    PWR_CL_26_360; 		                    	  /* [203] 	   */
			__IO uint8_t    Reserved19;		                        	  /* [204] 	   */
			__IO uint8_t    MIN_PERF_R_4_26;		                	  /* [205] 	   */
			__IO uint8_t    MIN_PERF_W_4_26; 		                	  /* [206] 	   */
			__IO uint8_t    MIN_PERF_R_8_26_4_52; 		            	  /* [207] 	   */
			__IO uint8_t    MIN_PERF_W_8_26_4_52; 		            	  /* [208] 	   */
			__IO uint8_t    MIN_PERF_R_8_52; 		                	  /* [209] 	   */
			__IO uint8_t    MIN_PERF_W_8_52; 		                	  /* [210] 	   */
			__IO uint8_t    SECURE_WP_INFO;		                    	  /* [211] 	   */
			__IO uint8_t    SEC_COUNT[4];		                    	  /* [212-215] */
			__IO uint8_t    SLEEP_NOTIFICATION_TIME;		        	  /* [216] 	   */
			__IO uint8_t    S_A_TIMEOUT;		                    	  /* [217] 	   */
			__IO uint8_t    PRODUCTION_STATE_AWARENESS_TIMEOUT;     	  /* [218] 	   */
			__IO uint8_t    S_C_VCCQ; 		                        	  /* [219] 	   */
			__IO uint8_t    S_C_VCC; 		                        	  /* [220] 	   */
			__IO uint8_t    HC_WP_GRP_SIZE; 		                	  /* [221] 	   */
			__IO uint8_t    REL_WR_SEC_C; 		                    	  /* [222] 	   */
			__IO uint8_t    ERASE_TIMEOUT_MULT; 		            	  /* [223] 	   */
			__IO uint8_t    HC_ERASE_GRP_SIZE; 		                	  /* [224] 	   */
			__IO uint8_t    ACC_SIZE; 		                        	  /* [225] 	   */
			__IO uint8_t    BOOT_SIZE_MULTI; 		                	  /* [226] 	   */
			__IO uint8_t    Reserved23;		                        	  /* [227] 	   */
			__IO uint8_t    BOOT_INFO; 		                        	  /* [228] 	   */
			__IO uint8_t    SEC_TRIM_MULT;		                    	  /* [229] 	   */
			__IO uint8_t    SEC_ERASE_MULT;		                    	  /* [230] 	   */
			__IO uint8_t    SEC_FEATURE_SUPPORT; 		            	  /* [231] 	   */
			__IO uint8_t    TRIM_MULT; 		                        	  /* [232] 	   */
			__IO uint8_t    Reserved24;		                        	  /* [233] 	   */
			__IO uint8_t    MIN_PERF_DDR_R_8_52;		            	  /* [234] 	   */
			__IO uint8_t    MIN_PERF_DDR_W_8_52; 		            	  /* [235] 	   */
			__IO uint8_t    PWR_CL_200_195; 		                	  /* [236] 	   */
			__IO uint8_t    PWR_CL_200_360; 		                	  /* [237] 	   */
			__IO uint8_t    PWR_CL_DDR_52_195; 		                	  /* [238] 	   */
			__IO uint8_t    PWR_CL_DDR_52_360; 		                	  /* [239] 	   */
			__IO uint8_t    CACHE_FLUSH_POLICY;		                	  /* [240] 	   */
			__IO uint8_t    INI_TIMEOUT_AP;		                    	  /* [241] 	   */
			__IO uint8_t    CORRECTLY_PRG_SECTORS_NUM[4];		    	  /* [242-245] */
			__IO uint8_t    BKOPS_STATUS;		                    	  /* [246]     */
			__IO uint8_t    POWER_OFF_LONG_TIME;		            	  /* [247] 	   */
			__IO uint8_t    GENERIC_CMD6_TIME; 		                	  /* [248] 	   */
			__IO uint8_t    CACHE_SIZE[4]; 		                    	  /* [249-252] */
            __IO uint8_t    PWR_CL_DDR_200_360;                     	  /* [253] 	   */
            __IO uint8_t    FIRMWARE_VERSION[8];                    	  /* [254-261] */
            __IO uint8_t    DEVICE_VERSION[2];                     		  /* [262-263] */
            __IO uint8_t    OPTIMAL_TRIM_UNIT_SIZE;                 	  /* [264] 	   */
            __IO uint8_t    OPTIMAL_WRITE_SIZE;                     	  /* [265] 	   */
            __IO uint8_t    OPTIMAL_READ_SIZE;                     		  /* [266] 	   */
            __IO uint8_t    PRE_EOL_INFO;                     			  /* [267] 	   */
            __IO uint8_t    DEVICE_LIFE_TIME_EST_TYP_A;             	  /* [268] 	   */
            __IO uint8_t    DEVICE_LIFE_TIME_EST_TYP_B;             	  /* [269] 	   */
            __IO uint8_t    VENDOR_PROPRIETARY_HEALTH_REPORT[32];   	  /* [270-301] */
            __IO uint8_t    NUMBER_OF_FW_SECTORS_CORRECTLY_PROGRAMMED[4]; /* [302-305] */
            __IO uint8_t    Reserved25;		                			  /* [306] 	   */
            __IO uint8_t    CMDQ_DEPTH;		                			  /* [307] 	   */
            __IO uint8_t    CMDQ_SUPPORT;		                		  /* [308] 	   */
            __IO uint8_t    Reserved26[177];		                	  /* [309-485] */
            __IO uint8_t    BARRIER_SUPPORT;		                	  /* [486] 	   */
            __IO uint8_t    FFU_ARG[4];		                			  /* [487-490] */
            __IO uint8_t    OPERATION_CODE_TIMEOUT;		            	  /* [491]	   */
            __IO uint8_t    FFU_FEATURES;		                		  /* [492]	   */
            __IO uint8_t    SUPPORTED_MODES;		                	  /* [493]	   */
			__IO uint8_t    EXT_SUPPORT;		                    	  /* [494]	   */
			__IO uint8_t    LARGE_UNIT_SIZE_M1; 		            	  /* [495]	   */
			__IO uint8_t    CONTEXT_CAPABILITIES; 		            	  /* [496]	   */
			__IO uint8_t    TAG_RES_SIZE; 		                    	  /* [497]	   */
			__IO uint8_t    TAG_UNIT_SIZE; 		                    	  /* [498]	   */
			__IO uint8_t    DATA_TAG_SUPPORT; 		                	  /* [499]	   */
			__IO uint8_t    MAX_PACKED_WRITES; 		                	  /* [500]	   */
			__IO uint8_t    MAX_PACKED_READS; 		                	  /* [501]	   */
			__IO uint8_t    BKOPS_SUPPORT;		                    	  /* [502]	   */
			__IO uint8_t    HPI_FEATURES; 		                    	  /* [503]	   */
			__IO uint8_t    S_CMD_SET;		                        	  /* [504]	   */
            __IO uint8_t    EXT_SECURITY_ERR;		                	  /* [505]	   */
			__IO uint8_t    Reserved27[6];	                        	  /* [506-511] */
		} EXT_CSD;
   __IO uint8_t CsdBuf[512];
} EMMCEXT_CSD;
#endif

/** 
  * @brief  Card Identification Data: CID Register  
  *         注意Reserved1与CBX以及后面的CRC7与Reserved2是两两顺序是反着的，
  *         不是写错了,原因是在一个字节中位域操作是先放低位后放高位,所以需要反向。
  *         之所以命名CRC7而不是CRC是由于其它地方已经定义了。
  */
typedef union
{
    struct {
        __IO uint8_t MID;           /*!<[127-120]   ManufacturerID */
        __IO uint8_t CBX:2;         /*!<[113-112]   Card/BGA */
        __IO uint8_t Reserved1:6;   /*!<[119-114]   Reserved */
        __IO uint8_t OID;           /*!<[111-104]   OEM/Application ID */
        __IO uint8_t PNM[6];        /*!<[103-56]    Product Name*/
        __IO uint8_t PRV;           /*!<[55-48]     Product Revision */
        __IO uint8_t PSN[4];        /*!<[47-16]     Product Serial Number */
        __IO uint8_t MDT;           /*!<[15-8]      Manufacturing Date */
        __IO uint8_t Reserved2:1;   /*!<[0]         always 1 */
        __IO uint8_t CRC7:7;        /*!<[7-1]       CID CRC */

    } cid;
    uint8_t buf[16];
} EMMC_CID;

/** 
  * @brief SD Card Status 
  */
typedef struct
{
  __IO uint8_t DAT_BUS_WIDTH;
  __IO uint8_t SECURED_MODE;
  __IO uint16_t SD_CARD_TYPE;
  __IO uint32_t SIZE_OF_PROTECTED_AREA;
  __IO uint8_t SPEED_CLASS;
  __IO uint8_t PERFORMANCE_MOVE;
  __IO uint8_t AU_SIZE;
  __IO uint16_t ERASE_SIZE;
  __IO uint8_t ERASE_TIMEOUT;
  __IO uint8_t ERASE_OFFSET;
} EMMC_CardStatus;


/** 
  * @brief SD Card information 
  */
typedef struct
{
  EMMC_CSD EMMC_Csd;
  EMMCEXT_CSD EMMC_ExtCsd; 
  EMMC_CID EMMC_Cid;
  uint64_t CardCapacity;  /*!< Card Capacity */
  uint32_t CardBlockSize; /*!< Card Block Size */
  uint16_t RCA;
  uint8_t CardType;
}EMMC_CardInfo;
   
/**
  * @}
  */

/** @defgroup SDIO_EMMC_Exported_Macros
  * @{
  */
#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40012C80)
/** 
  * @brief  SDIO Intialization Frequency (400KHz max)
  */
#define SDIO_INIT_CLK_DIV                ((uint8_t)0xFF)
/** 
  * @brief  SDIO Data Transfer Frequency (25MHz max) 
  */
#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x02) 

#define SD_SDIO_DMA                   DMA2
#define SD_SDIO_DMA_CLK               RCC_AHB1Periph_DMA2
 
#define EMMC_SDIO_DMA_STREAM3	      3
//#define EMMC_SDIO_DMA_STREAM6       6


#ifdef EMMC_SDIO_DMA_STREAM3
 #define EMMC_SDIO_DMA_STREAM            DMA2_Stream3
 #define EMMC_SDIO_DMA_CHANNEL           DMA_Channel_4
 #define EMMC_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF3
 #define EMMC_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF3
 #define EMMC_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF3
 #define EMMC_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF3
 #define EMMC_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF3 
 #define EMMC_SDIO_DMA_IRQn              DMA2_Stream3_IRQn
#ifdef USE_EMMC
 #define EMMC_SDIO_IRQHandler            SDIO_IRQHandler
 #define EMMC_SDIO_DMA_IRQHANDLER        DMA2_Stream3_IRQHandler 
#endif /* USE_SDCARD */
#elif defined EMMC_SDIO_DMA_STREAM6
 #define EMMC_SDIO_DMA_STREAM            DMA2_Stream6
 #define EMMC_SDIO_DMA_CHANNEL           DMA_Channel_4
 #define EMMC_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
 #define EMMC_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
 #define EMMC_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
 #define EMMC_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
 #define EMMC_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF6 
 #define EMMC_SDIO_DMA_IRQn              DMA2_Stream6_IRQn
 #define EMMC_SDIO_DMA_IRQHANDLER        DMA2_Stream6_IRQHandler
#endif /* EMMC_SDIO_DMA_STREAM3 */

/** 
  * @brief SDIO Commands  Index 
  */
#define EMMC_CMD_GO_IDLE_STATE                       ((uint8_t)0)
#define EMMC_CMD_SEND_OP_COND                        ((uint8_t)1)
#define EMMC_CMD_ALL_SEND_CID                        ((uint8_t)2)
#define EMMC_CMD_SET_REL_ADDR                        ((uint8_t)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define EMMC_CMD_SET_DSR                             ((uint8_t)4)
#define EMMC_CMD_SDIO_SEN_OP_COND                    ((uint8_t)5)
#define EMMC_CMD_HS_SWITCH                           ((uint8_t)6)
#define EMMC_CMD_SEL_DESEL_CARD                      ((uint8_t)7)
#define EMMC_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8)
#define EMMC_CMD_SEND_CSD                            ((uint8_t)9)
#define EMMC_CMD_SEND_CID                            ((uint8_t)10)
#define EMMC_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /*!< SD Card doesn't support it */
#define EMMC_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define EMMC_CMD_SEND_STATUS                         ((uint8_t)13)
#define EMMC_CMD_HS_BUSTEST_READ                     ((uint8_t)14)
#define EMMC_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)
#define EMMC_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define EMMC_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define EMMC_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define EMMC_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define EMMC_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20) /*!< SD Card doesn't support it */
#define EMMC_CMD_SET_BLOCK_COUNT                     ((uint8_t)23) /*!< SD Card doesn't support it */
#define EMMC_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define EMMC_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define EMMC_CMD_PROG_CID                            ((uint8_t)26) /*!< reserved for manufacturers */
#define EMMC_CMD_PROG_CSD                            ((uint8_t)27)
#define EMMC_CMD_SET_WRITE_PROT                      ((uint8_t)28)
#define EMMC_CMD_CLR_WRITE_PROT                      ((uint8_t)29)
#define EMMC_CMD_SEND_WRITE_PROT                     ((uint8_t)30)
#define EMMC_CMD_ERASE_GRP_START                     ((uint8_t)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define EMMC_CMD_ERASE_GRP_END                       ((uint8_t)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define EMMC_CMD_ERASE                               ((uint8_t)38)
#define EMMC_CMD_FAST_IO                             ((uint8_t)39) /*!< SD Card doesn't support it */
#define EMMC_CMD_GO_IRQ_STATE                        ((uint8_t)40) /*!< SD Card doesn't support it */
#define EMMC_CMD_LOCK_UNLOCK                         ((uint8_t)42)
#define EMMC_CMD_APP_CMD                             ((uint8_t)55)
#define EMMC_CMD_GEN_CMD                             ((uint8_t)56)
#define EMMC_CMD_NO_CMD                              ((uint8_t)64)



  
/* Uncomment the following line to select the SDIO Data transfer mode */  
#if !defined (SD_DMA_MODE) && !defined (SD_POLLING_MODE)
#define SD_DMA_MODE                                ((uint32_t)0x00000000)
//#define SD_POLLING_MODE                            ((uint32_t)0x00000002)
#endif

/**
  * @brief  SD detection on its memory slot
  */
#define SD_PRESENT                                 ((uint8_t)0x01)
#define SD_NOT_PRESENT                             ((uint8_t)0x00)

/** 
  * @brief Supported SD Memory Cards 
  */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)

  
/** 
  * @brief  SDIO Static flags, TimeOut, FIFO Address  
  */
#define NULL 0
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x01000000)

/** 
  * @brief  Mask for errors Card Status R1 (OCR Register) 
  */
#define EMMC_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define EMMC_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define EMMC_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define EMMC_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define EMMC_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define EMMC_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define EMMC_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define EMMC_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define EMMC_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define EMMC_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define EMMC_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define EMMC_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define EMMC_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define EMMC_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define EMMC_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define HOCR_WP_ERASE_SKIP                ((uint32_t)0x00008000)
#define EMMC_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define EMMC_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define EMMC_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define EMMC_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

/** 
  * @brief  Masks for R6 Response 
  */
#define EMMC_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define EMMC_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define EMMC_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define EMMC_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define EMMC_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define EMMC_STD_CAPACITY                 ((uint32_t)0x00000000)
#define EMMC_CHECK_PATTERN                ((uint32_t)0x000001AA)

#define EMMC_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define EMMC_ALLZERO                      ((uint32_t)0x00000000)

#define EMMC_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define EMMC_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define EMMC_CARD_LOCKED                  ((uint32_t)0x02000000)

#define EMMC_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
#define EMMC_0TO7BITS                     ((uint32_t)0x000000FF)
#define EMMC_8TO15BITS                    ((uint32_t)0x0000FF00)
#define EMMC_16TO23BITS                   ((uint32_t)0x00FF0000)
#define EMMC_24TO31BITS                   ((uint32_t)0xFF000000)
#define EMMC_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define EMMC_HALFFIFO                     ((uint32_t)0x00000008)
#define EMMC_HALFFIFOBYTES                ((uint32_t)0x00000020)

/** 
  * @brief  Command Class Supported 
  */
#define EMMC_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080)
#define EMMC_CCCC_WRITE_PROT              ((uint32_t)0x00000040)
#define EMMC_CCCC_ERASE                   ((uint32_t)0x00000020)

 
/**
  * @}
  */ 

#define EMMC_OCR_REG             0x40FF8080
#define EMMC_POWER_REG           0x03BB0800
#define EMMC_HIGHSPEED_REG       0x03B90100
#define EMMC_4BIT_REG            0x03B70100
#define EMMC_8BIT_REG            0x03B70200

/** @defgroup SDIO_EMMC_Exported_Functions
  * @{
  */ 
EMMC_Error EMMC_Init(void);
EMMC_Error EMMC_CmdError(void);
EMMC_Error EMMC_CmdResp1Error(uint8_t cmd);
EMMC_Error EMMC_CmdResp7Error(void);
EMMC_Error EMMC_CmdResp3Error(void);
EMMC_Error EMMC_CmdResp2Error(void);
EMMC_Error EMMC_CmdResp6Error(uint8_t cmd, uint16_t *prca);
EMMC_Error EMMC_EnWideBus(FunctionalState NewState);
EMMC_Error EMMC_IsCardProgramming(uint8_t *pstatus);
EMMC_Error EMMC_PowerON(void);
EMMC_Error EMMC_InitializeCards(EMMC_CardInfo *E);
EMMC_Error EMMC_SendStatus(uint32_t *pcardstatus);
EMMC_Error EMMC_StopTransfer(void);
EMMC_Error EMMC_GetCardInfo(EMMC_CardInfo *E, uint32_t *CSD_Tab, uint32_t *CID_Tab, uint16_t Rca);
EMMC_Error EMMC_SelectDeselect(uint32_t addr);
EMMC_Error EMMC_ProcessIRQSrc(void);
EMMC_Error EMMC_ReadExtCsd(EMMC_CardInfo *E);
EMMC_Error EMMC_EnableWideBusOperation(uint32_t WideMode);
EMMC_Error EMMC_ReadBlock(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize);
EMMC_Error EMMC_ReadMultiBlocks(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
EMMC_Error EMMC_WriteBlock(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize);
EMMC_Error EMMC_WriteMultiBlocks(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
EMMC_Error EMMC_Erase(uint32_t startaddr, uint32_t endaddr);
EMMCCardState EMMC_GetState(void);
EMMCTransferState EMMC_GetStatus(void);
EMMC_Error EMMC_WaitReadOperation(void);
EMMC_Error EMMC_WaitWriteOperation(void);
void EMMC_ProcessDMAIRQ(void);
void EMMC_LowLevelDMATxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void EMMC_LowLevelDMARxConfig(uint32_t *BufferDST, uint32_t BufferSize);
uint8_t emmc_convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);

extern EMMC_CardInfo MyEMMC_CardInfo;
/**
  * @}
  */ 
#ifdef __cplusplus
}
#endif

#endif /* __EMMC_SDIO_H */



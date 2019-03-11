#ifndef DRV_EMMC_H
#define DRV_EMMC_H

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
} EMMC_ERR;



#endif


/**
  ******************************************************************************
  * @file    drv_emmc.c
  * @author  HouXianxin 
  * @version V1.0.0
  * @date    2019/02/13
  * @brief   This file is bsp_sdio_emmc drive file.
  ******************************************************************************
  *          =========================
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SDIO Pins            |   EMMC        |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          |      SDIO D2                |   D2          |    1        |
  *          |      SDIO D3                |   D3          |    2        |
  *          |      SDIO CMD               |   CMD         |    3        |
  *          |                             |   VCC         |    4 (3.3 V)|
  *          |      SDIO CLK               |   CLK         |    5        |
  *          |                             |   GND         |    6 (0 V)  |
  *          |      SDIO D0                |   D0          |    7        |
  *          |      SDIO D1                |   D1          |    8        |
  *          +-----------------------------+---------------+-------------+
  *
  *  @endverbatim
  */
	
/** @defgroup SDIO_EMMC_Private_Types
* @{
*/ 
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "drv_emmc.h"
#include "drv_delay.h"
#include "includes.h"

#define DRV_EMMC_DEBUG printf

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SDIO_InitTypeDef EMMC_SDIO_InitStructure;
SDIO_CmdInitTypeDef EMMC_SDIO_CmdInitStructure;
SDIO_DataInitTypeDef EMMC_SDIO_DataInitStructure;
__IO uint32_t EMMC_StopCondition = 0;
__IO uint32_t EMMC_TransferEnd = 0;
__IO uint32_t EMMC_DMAEndOfTransfer = 0;
__IO EMMC_Error EMMC_TransferError;
EMMC_CardInfo MyEMMC_CardInfo;
static uint32_t CardType =  SDIO_STD_CAPACITY_SD_CARD_V1_1;
//static uint8_t SDSTATUS_Tab[16];
/* Private functions ---------------------------------------------------------*/
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
EMMC_Error EMMC_GetCardInfo2(EMMC_CardInfo *E, uint32_t *CSD_Tab, uint32_t *CID_Tab, uint16_t Rca);
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
EMMC_Error EMMC_WaitReadOperation(void);
EMMC_Error EMMC_WaitWriteOperation(void);
void EMMC_ProcessDMAIRQ(void);
void EMMC_LowLevelDMATxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void EMMC_LowLevelDMARxConfig(uint32_t *BufferDST, uint32_t BufferSize);
uint8_t emmc_convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);
/**
  * @}
  */ 

/**
  * @brief  Configures SDIO IRQ channel.
  * @param  None
  * @retval None
  */
void EMMC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
 
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EMMC_SDIO_DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_Init(&NVIC_InitStructure);  
}

/**
  * @brief  Initializes the EMMC_ and put it into StandBy State (Ready for 
  *         data transfer).
  * @param  None
  * @retval None
  */
void EMMC_PeriphInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* GPIOC and GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);

  /* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Configure PC.12 pin: CLK pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Enable the SDIO APB2 Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

  /* Enable the DMA2 Clock */
  RCC_AHB1PeriphClockCmd(SD_SDIO_DMA_CLK, ENABLE);
}

/**
  * @brief  Configures the DMA2 Channel4 for SDIO Tx request.
  * @param  BufferSRC: pointer to the source buffer
  * @param  BufferSize: buffer size
  * @retval None
  */
void EMMC_LowLevelDMATxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_ClearFlag(EMMC_SDIO_DMA_STREAM, EMMC_SDIO_DMA_FLAG_FEIF | EMMC_SDIO_DMA_FLAG_DMEIF | EMMC_SDIO_DMA_FLAG_TEIF | EMMC_SDIO_DMA_FLAG_HTIF | EMMC_SDIO_DMA_FLAG_TCIF);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(EMMC_SDIO_DMA_STREAM, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(EMMC_SDIO_DMA_STREAM);

  DMA_InitStructure.DMA_Channel = EMMC_SDIO_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(EMMC_SDIO_DMA_STREAM, &DMA_InitStructure);
  DMA_ITConfig(EMMC_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_FlowControllerConfig(EMMC_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(EMMC_SDIO_DMA_STREAM, ENABLE);
    

}

/**
  * @brief  Configures the DMA2 Channel4 for SDIO Rx request.
  * @param  BufferDST: pointer to the destination buffer
  * @param  BufferSize: buffer size
  * @retval None
  */
void EMMC_LowLevelDMARxConfig(uint32_t *BufferDST, uint32_t BufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_ClearFlag(EMMC_SDIO_DMA_STREAM, EMMC_SDIO_DMA_FLAG_FEIF | EMMC_SDIO_DMA_FLAG_DMEIF | EMMC_SDIO_DMA_FLAG_TEIF | EMMC_SDIO_DMA_FLAG_HTIF | EMMC_SDIO_DMA_FLAG_TCIF);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(EMMC_SDIO_DMA_STREAM, DISABLE);

  /* DMA2 Stream3 or Stream6 Config */
  DMA_DeInit(EMMC_SDIO_DMA_STREAM);

  DMA_InitStructure.DMA_Channel = EMMC_SDIO_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferDST;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(EMMC_SDIO_DMA_STREAM, &DMA_InitStructure);
  DMA_ITConfig(EMMC_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_FlowControllerConfig(EMMC_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3 or Stream6 enable */
  DMA_Cmd(EMMC_SDIO_DMA_STREAM, ENABLE);
}


/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for data 
  *         transfer).
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_Init(void)
{
  __IO EMMC_Error Result = EMMC_OK;
  EMMC_NVIC_Configuration();
  EMMC_PeriphInit();
  SDIO_DeInit();

  Result = EMMC_PowerON();
  if (Result != EMMC_OK)
  {
    return(Result);
  }
  
//  Result = EMMC_PowerON();
//  if (Result != EMMC_OK)
//  {
//    return(Result);
//  }
  Result = EMMC_InitializeCards(&MyEMMC_CardInfo);
  //printf("EMMC TOTAL SIZE:%lld byte\r\n", MyEMMC_CardInfo.CardCapacity);
  if (Result != EMMC_OK)
  {
    return(Result);
  }
//  /*!< Configure the SDIO peripheral */
//  /*!< SDIO_CK = SDIOCLK / (SDIO_TRANSFER_CLK_DIV + 2) */
//  /*!< on STM32F4xx devices, SDIOCLK is fixed to 48MHz */
//  EMMC_SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
//  EMMC_SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
//  EMMC_SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
//  EMMC_SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
//  EMMC_SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
//  EMMC_SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
//  SDIO_Init(&EMMC_SDIO_InitStructure);
   if (Result == EMMC_OK)
   {
     Result = EMMC_EnableWideBusOperation(SDIO_BusWide_4b);
   }  
    printf("emmc bus init %d\r\n", Result);
  return(Result);
}

/**
  * @brief  Gets the cuurent sd card data transfer status.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
EMMCTransferState EMMC_GetStatus(void)
{
		EMMCCardState CardState =  EMMC_CARD_TRANSFER;

		CardState = EMMC_GetState();
		
		if (CardState == EMMC_CARD_TRANSFER)
		{
			return(EMMC_TRANSFER_OK);
		}
		else if (CardState == EMMC_CARD_RECEIVING)
		{
			return(EMMC_TRANSFER_OK);
		}
		
		else if(CardState == EMMC_CARD_ERROR)
		{
			return (EMMC_TRANSFER_ERROR);
		}
		else
		{
			return(EMMC_TRANSFER_BUSY);
		}
}

/**
  * @brief  Returns the current card's state.
  * @param  None
  * @retval EMMC_CardState: EMMC Card Error or EMMC Card Current State.
  */
EMMCCardState EMMC_GetState(void)
{
    uint32_t Resp = 0;
	
    if (EMMC_SendStatus(&Resp) != EMMC_OK)
    {
      return EMMC_CARD_ERROR;
    }
    else
    {
      return (EMMCCardState)((Resp >> 9) & 0x0F);
    }
}



/**
  * @brief  Enquires cards about their operating voltage and configures 
  *   clock controls.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_PowerON(void)
{
    __IO EMMC_Error Result = EMMC_OK; 
    uint32_t Cnt = 0;

    /*!< Power ON Sequence -----------------------------------------------------*/
    /*!< Configure the SDIO peripheral */
    /*!< SDIO_CK = SDIOCLK / (SDIO_INIT_CLK_DIV + 2) */
    /*!< on STM32F4xx devices, SDIOCLK is fixed to 48MHz */
    /*!< SDIO_CK for initialization should not exceed 400 KHz */  
    EMMC_SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
    EMMC_SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    EMMC_SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    EMMC_SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    EMMC_SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    EMMC_SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_Init(&EMMC_SDIO_InitStructure);
    
    /*!< Set Power State to ON */
    SDIO_SetPowerState(SDIO_PowerState_ON);
    
    /*!< Enable SDIO Clock */
    SDIO_ClockCmd(ENABLE);
     
    /*!< CMD0: GO_IDLE_STATE ---------------------------------------------------*/
    /*!< No CMD response required */
    EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_GO_IDLE_STATE;
    EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
    EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

    Result = EMMC_CmdError();
  
    if (Result != EMMC_OK) {
        DRV_EMMC_DEBUG("CMD0 Failed %d\r\n", Result);
        return(Result);
    }
    DRV_EMMC_DEBUG("CMD0 OK\r\n");
	CardType = SDIO_HIGH_CAPACITY_MMC_CARD; 
	do
	{
        /*!< SEND CMD1*/
        EMMC_SDIO_CmdInitStructure.SDIO_Argument = EMMC_OCR_REG;
        EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SEND_OP_COND;
        EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);
        Result = EMMC_CmdResp3Error();
        Cnt++;
	}while((Cnt<400)&& (Result != EMMC_OK));
	
    drv_delay_ms(200);
	/*!< SEND CMD1*/
	EMMC_SDIO_CmdInitStructure.SDIO_Argument = EMMC_OCR_REG;
	EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SEND_OP_COND;
	EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);
	Result = EMMC_CmdResp3Error();
    if(Result != EMMC_OK) {
        DRV_EMMC_DEBUG("CMD1 Failed %d\r\n", Result);
    }
    DRV_EMMC_DEBUG("CMD1 OK\r\n");
    return(Result);
}

/**
  * @brief  Turns the SDIO output signals off.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_PowerOFF(void)
{
  EMMC_Error errorstatus = EMMC_OK;

  /*!< Set Power State to OFF */
  SDIO_SetPowerState(SDIO_PowerState_OFF);

  return(errorstatus);
}

/**
  * @brief  Intialises all cards or single card as the case may be Card(s) come 
  *         into standby state.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_InitializeCards(EMMC_CardInfo *E)
{
    EMMC_Error Result = EMMC_OK;
	uint32_t CSD_Tab[4] = {0};
	uint32_t CID_Tab[4] = {0};
	uint16_t Rca = 0x01;
  

  if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
        Result = EMMC_REQUEST_NOT_APPLICABLE;
        return(Result);
  }
    /*!< Send CMD2 ALL_SEND_CID */
    EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_ALL_SEND_CID;
    EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);
		
    Result = EMMC_CmdResp2Error();

    if (EMMC_OK != Result)
    {
        DRV_EMMC_DEBUG("CMD2 Failed %d\r\n", Result);
        return(Result);
    }
    DRV_EMMC_DEBUG("CMD2 OK\r\n");
    CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
    /*!< Send CMD3 SET_REL_ADDR with argument 0 */
    /*!< SD Card publishes its RCA. */
    EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SET_REL_ADDR;
    EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

    Result = EMMC_CmdResp6Error(EMMC_CMD_SET_REL_ADDR, &Rca);

    if (EMMC_OK != Result)
    {
        DRV_EMMC_DEBUG("CMD3 Failed %d\r\n", Result);
        return(Result);
    }
    DRV_EMMC_DEBUG("CMD3 OK\r\n");
    /*!< Send CMD9 SEND_CSD with argument as card's RCA */
    EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(Rca << 16);
    EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SEND_CSD;
    EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

    Result = EMMC_CmdResp2Error();

    if (EMMC_OK != Result)
    {
        DRV_EMMC_DEBUG("CMD9 Failed: %d\r\n", Result);
        return(Result);
    }
    DRV_EMMC_DEBUG("CMD9 OK\r\n");
    CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);	

	Result = EMMC_SelectDeselect((uint32_t) (MyEMMC_CardInfo.RCA << 16));
	if (EMMC_OK != Result)
	{
        DRV_EMMC_DEBUG("SDeselect Failed: %d\r\n", Result);
		return(Result);
	}
    DRV_EMMC_DEBUG("SDeselect OK\r\n");
	Result = EMMC_ReadExtCsd(&MyEMMC_CardInfo);
	if (EMMC_OK != Result)
	{
        DRV_EMMC_DEBUG("ReadExt Failed: %d\r\n", Result);
		return(Result);
	}
    DRV_EMMC_DEBUG("ReadExt OK\r\n");
    Result = EMMC_GetCardInfo2(E, &CSD_Tab[0], &CID_Tab[0], Rca);
	if (EMMC_OK != Result)
	{
		return(Result);
	}
  Result = EMMC_OK; /*!< All cards get intialized */
  return(Result);
}

#define HTONL(v) ( ((v) << 24) | (((v) >> 24) & 255) | (((v) << 8) & 0xff0000) | (((v) >> 8) & 0xff00) )
EMMC_Error EMMC_HTON(uint32_t *dst, uint32_t* src, uint32_t n)
{
    EMMC_Error errorstatus = EMMC_OK;
    
    for(uint32_t i = 0; i < n; i++)
        dst[i] = HTONL(src[i]);
    return errorstatus;
}

EMMC_Error EMMC_GetCardInfo2(EMMC_CardInfo *E, uint32_t *CSD_Tab, uint32_t *CID_Tab, uint16_t Rca)
{
    uint32_t cid_buf[4] = {0};
    uint32_t csd_buf[4] = {0};
    
    EMMC_Error errorstatus = EMMC_OK;

    /* CID INFO */
    EMMC_HTON((uint32_t*)cid_buf, CID_Tab, 4);
    memcpy(E->EMMC_Cid.buf, cid_buf, sizeof(E->EMMC_Cid.buf));
    
    /* CSD INFO */
    memcpy(csd_buf, CSD_Tab, sizeof(csd_buf));
    E->EMMC_Csd.CSD_STRUCTURE      = (csd_buf[0] & 0xC0000000) >> 30;
    E->EMMC_Csd.SPEC_VERS          = (csd_buf[0] & 0x3C000000) >> 26;
    E->EMMC_Csd.Reserved1          = (csd_buf[0] & 0x03000000) >> 24;
    E->EMMC_Csd.TAAC               = (csd_buf[0] & 0x00FF0000) >> 16;
    E->EMMC_Csd.NSAC               = (csd_buf[0] & 0x0000FF00) >> 8;
    E->EMMC_Csd.TRAN_SPEED         = (csd_buf[0] & 0x000000FF) >> 0;
    E->EMMC_Csd.CCC                = (csd_buf[1] & 0xFFF00000) >> 20;
    E->EMMC_Csd.READ_BL_LEN        = (csd_buf[1] & 0x000F0000) >> 16;
    E->EMMC_Csd.READ_BL_PARTIAL    = (csd_buf[1] & 0x00008000) >> 15;
    E->EMMC_Csd.WRITE_BLK_MISALIGN = (csd_buf[1] & 0x00004000) >> 14;
    E->EMMC_Csd.READ_BLK_MISALIGN  = (csd_buf[1] & 0x00002000) >> 13;
    E->EMMC_Csd.DSR_IMP            = (csd_buf[1] & 0x00001000) >> 12;
    E->EMMC_Csd.Reserved2          = (csd_buf[1] & 0x00000C00) >> 10;
    E->EMMC_Csd.C_SIZE             = (csd_buf[1] & 0x000003FF) << 2 | (csd_buf[2] & 0xC0000000) >> 30;
    E->EMMC_Csd.VDD_R_CURR_MIN     = (csd_buf[2] & 0x38000000) >> 27;
    E->EMMC_Csd.VDD_R_CURR_MAX     = (csd_buf[2] & 0x07000000) >> 24;
    E->EMMC_Csd.VDD_W_CURR_MIN     = (csd_buf[2] & 0x00E00000) >> 21;
    E->EMMC_Csd.VDD_W_CURR_MAX     = (csd_buf[2] & 0x001C0000) >> 18;
    E->EMMC_Csd.C_SIZE_MULT        = (csd_buf[2] & 0x00038000) >> 15;
    E->EMMC_Csd.ERASE_GRP_SIZE     = (csd_buf[2] & 0x00007C00) >> 10;
    E->EMMC_Csd.ERASE_GRP_MULT     = (csd_buf[2] & 0x000003E0) >> 5;
    E->EMMC_Csd.WP_GRP_SIZE        = (csd_buf[2] & 0x0000001F) >> 0;
    E->EMMC_Csd.WP_GRP_ENABLE      = (csd_buf[3] & 0x80000000) >> 31;
    E->EMMC_Csd.DEFAULT_ECC        = (csd_buf[3] & 0x60000000) >> 29;
    E->EMMC_Csd.R2W_FACTOR         = (csd_buf[3] & 0x1C000000) >> 26;
    E->EMMC_Csd.WRITE_BL_LEN       = (csd_buf[3] & 0x03C00000) >> 22;
    E->EMMC_Csd.WRITE_BL_PARTIAL   = (csd_buf[3] & 0x00200000) >> 21;
    E->EMMC_Csd.Reserved3          = (csd_buf[3] & 0x001E0000) >> 17;
    E->EMMC_Csd.CONTENT_PROT_APP   = (csd_buf[3] & 0x00010000) >> 16;
    E->EMMC_Csd.FILE_FORMAT_GRP    = (csd_buf[3] & 0x00008000) >> 15;
    E->EMMC_Csd.COPY               = (csd_buf[3] & 0x00004000) >> 14;
    E->EMMC_Csd.PERM_WRITE_PROTECT = (csd_buf[3] & 0x00002000) >> 13;
    E->EMMC_Csd.TMP_WRITE_PROTECT  = (csd_buf[3] & 0x00001000) >> 12;
    E->EMMC_Csd.FILE_FORMAT        = (csd_buf[3] & 0x00000C00) >> 10;
    E->EMMC_Csd.ECC                = (csd_buf[3] & 0x00000300) >> 8;
    E->EMMC_Csd.CSD_CRC            = (csd_buf[3] & 0x000000FE) >> 1;
    E->EMMC_Csd.Reserved4          = (csd_buf[3] & 0x00000001) >> 0;
    
    /* OTHER */
    E->CardType = (uint8_t)CardType;
    E->RCA = (uint16_t)Rca;
    E->CardBlockSize = 1 << (E->EMMC_Csd.READ_BL_LEN);
    E->CardCapacity = (uint64_t)((uint64_t)(E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[3] << 24 | E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[2] << 16 | E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[1] << 8 | E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[0]) * E->CardBlockSize);
    return errorstatus;
}

/**
  * @brief  Returns information about specific card.
  * @param  E: pointer to a SD_CardInfo structure that contains all SD card 
  *         information.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error  EMMC_GetCardInfo(EMMC_CardInfo *E, uint32_t *CSD_Tab, uint32_t *CID_Tab, uint16_t Rca)
{
	
		EMMC_Error errorstatus = EMMC_OK;
		uint8_t tmp = 0;


		E->CardType = (uint8_t)CardType;
		E->RCA = (uint16_t)Rca;

		/*!< Byte 0 */
		tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
		E->EMMC_Csd.CSD_STRUCTURE = (tmp & 0xC0) >> 6;
		E->EMMC_Csd.SPEC_VERS = (tmp & 0x3C) >> 2;
		E->EMMC_Csd.Reserved1 = tmp & 0x03;

		/*!< Byte 1 */
		tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);
		E->EMMC_Csd.TAAC = tmp;

		/*!< Byte 2 */
		tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);
		E->EMMC_Csd.NSAC = tmp;

		/*!< Byte 3 */
		tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);
		E->EMMC_Csd.TRAN_SPEED = tmp;

		/*!< Byte 4 */
		tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);
		E->EMMC_Csd.CCC = tmp << 4;

		/*!< Byte 5 */
		tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);
		E->EMMC_Csd.CCC |= (tmp & 0xF0) >> 4;
		E->EMMC_Csd.READ_BL_LEN = tmp & 0x0F;

		/*!< Byte 6 */
		tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);
		E->EMMC_Csd.READ_BL_PARTIAL = (tmp & 0x80) >> 7;
		E->EMMC_Csd.WRITE_BLK_MISALIGN = (tmp & 0x40) >> 6;
		E->EMMC_Csd.READ_BLK_MISALIGN = (tmp & 0x20) >> 5;
		E->EMMC_Csd.DSR_IMP = (tmp & 0x10) >> 4;
		E->EMMC_Csd.Reserved2 = 0; /*!< Reserved */


		E->EMMC_Csd.C_SIZE = (tmp & 0x03) << 10;

		/*!< Byte 7 */
		tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
		E->EMMC_Csd.C_SIZE |= (tmp) << 2;

		/*!< Byte 8 */
		tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
		E->EMMC_Csd.C_SIZE |= (tmp & 0xC0) >> 6;

		E->EMMC_Csd.VDD_R_CURR_MIN = (tmp & 0x38) >> 3;
		E->EMMC_Csd.VDD_R_CURR_MAX = (tmp & 0x07);

		/*!< Byte 9 */
		tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
		E->EMMC_Csd.VDD_W_CURR_MIN = (tmp & 0xE0) >> 5;
		E->EMMC_Csd.VDD_W_CURR_MAX = (tmp & 0x1C) >> 2;
		E->EMMC_Csd.C_SIZE_MULT = (tmp & 0x03) << 1;
		/*!< Byte 10 */
		tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
		E->EMMC_Csd.C_SIZE_MULT |= (tmp & 0x80) >> 7;


		E->CardBlockSize = 1 << (E->EMMC_Csd.READ_BL_LEN);


		E->CardCapacity = (uint64_t)((uint64_t)(E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[3] << 24 | E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[2] << 16 | E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[1] << 8 | E->EMMC_ExtCsd.EXT_CSD.SEC_COUNT[0]) * E->CardBlockSize);


		E->EMMC_Csd.ERASE_GRP_SIZE = (tmp & 0x40) >> 6;
		E->EMMC_Csd.ERASE_GRP_MULT = (tmp & 0x3F) << 1;

		/*!< Byte 11 */
		tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);
		E->EMMC_Csd.ERASE_GRP_MULT |= (tmp & 0x80) >> 7;
		E->EMMC_Csd.WP_GRP_SIZE = (tmp & 0x7F);

		/*!< Byte 12 */
		tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);
		E->EMMC_Csd.WP_GRP_ENABLE = (tmp & 0x80) >> 7;
		E->EMMC_Csd.DEFAULT_ECC = (tmp & 0x60) >> 5;
		E->EMMC_Csd.R2W_FACTOR = (tmp & 0x1C) >> 2;
		E->EMMC_Csd.WRITE_BL_LEN = (tmp & 0x03) << 2;

		/*!< Byte 13 */
		tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);
		E->EMMC_Csd.WRITE_BL_LEN |= (tmp & 0xC0) >> 6;
		E->EMMC_Csd.WRITE_BL_PARTIAL = (tmp & 0x20) >> 5;
		E->EMMC_Csd.Reserved3 = 0;
		E->EMMC_Csd.CONTENT_PROT_APP = (tmp & 0x01);

		/*!< Byte 14 */
		tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);
		E->EMMC_Csd.FILE_FORMAT_GRP = (tmp & 0x80) >> 7;
		E->EMMC_Csd.COPY = (tmp & 0x40) >> 6;
		E->EMMC_Csd.PERM_WRITE_PROTECT = (tmp & 0x20) >> 5;
		E->EMMC_Csd.TMP_WRITE_PROTECT = (tmp & 0x10) >> 4;
		E->EMMC_Csd.FILE_FORMAT = (tmp & 0x0C) >> 2;
		E->EMMC_Csd.ECC = (tmp & 0x03);

		/*!< Byte 15 */
		tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF);
		E->EMMC_Csd.CSD_CRC = (tmp & 0xFE) >> 1;
		E->EMMC_Csd.Reserved4 = 1;

#if 0
		/*!< Byte 0 */
		tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);
		E->EMMC_Cid.cid.MID = tmp;

		/*!< Byte 1 */
		tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);
		E->EMMC_Cid.cid.OID = tmp << 8;

		/*!< Byte 2 */
		tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);
		E->EMMC_Cid.cid.OID |= tmp;

		/*!< Byte 3 */
		tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);
		E->EMMC_Cid.cid.PNM = tmp << 24;

		/*!< Byte 4 */
		tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);
		E->EMMC_Cid.ProdName1 |= tmp << 16;

		/*!< Byte 5 */
		tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);
		E->EMMC_Cid.ProdName1 |= tmp << 8;

		/*!< Byte 6 */
		tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);
		E->EMMC_Cid.ProdName1 |= tmp;

		/*!< Byte 7 */
		tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);
		E->EMMC_Cid.ProdName2 = tmp;

		/*!< Byte 8 */
		tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);
		E->EMMC_Cid.ProdRev = tmp;

		/*!< Byte 9 */
		tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);
		E->EMMC_Cid.ProdSN = tmp << 24;

		/*!< Byte 10 */
		tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);
		E->EMMC_Cid.ProdSN |= tmp << 16;

		/*!< Byte 11 */
		tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);
		E->EMMC_Cid.ProdSN |= tmp << 8;

		/*!< Byte 12 */
		tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);
		E->EMMC_Cid.ProdSN |= tmp;

		/*!< Byte 13 */
		tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);
		E->EMMC_Cid.Reserved1 |= (tmp & 0xF0) >> 4;
		E->EMMC_Cid.ManufactDate = (tmp & 0x0F) << 8;

		/*!< Byte 14 */
		tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);
		E->EMMC_Cid.ManufactDate |= tmp;

		/*!< Byte 15 */
		tmp = (uint8_t)(CID_Tab[3] & 0x000000FF);
		E->EMMC_Cid.CID_CRC = (tmp & 0xFE) >> 1;
		E->EMMC_Cid.Reserved2 = 1;
#endif
  return(errorstatus);
}


/**
  * @brief  Enables wide bus opeartion for the requeseted card if supported by 
  *         card.
  * @param  WideMode: Specifies the SD card wide bus mode. 
  *   This parameter can be one of the following values:
  *     @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
  *     @arg SDIO_BusWide_4b: 4-bit data transfer
  *     @arg SDIO_BusWide_1b: 1-bit data transfer
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_EnableWideBusOperation(uint32_t WideMode)
{
  EMMC_Error errorstatus = EMMC_OK;


    if (SDIO_BusWide_8b == WideMode)
    {
      errorstatus = EMMC_UNSUPPORTED_FEATURE;
      return(errorstatus);
    }
    else if (SDIO_BusWide_4b == WideMode)
    {
      errorstatus = EMMC_EnWideBus(ENABLE);

      if (EMMC_OK == errorstatus)
      {
        /*!< Configure the SDIO peripheral */
        EMMC_SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        EMMC_SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        EMMC_SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        EMMC_SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        EMMC_SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;
        EMMC_SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&EMMC_SDIO_InitStructure);
      }
    }
    else
    {
      errorstatus = EMMC_EnWideBus(DISABLE);

      if (EMMC_OK == errorstatus)
      {
        /*!< Configure the SDIO peripheral */
        EMMC_SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        EMMC_SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        EMMC_SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        EMMC_SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        EMMC_SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
        EMMC_SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&EMMC_SDIO_InitStructure);
      }
    }
//  }

  return(errorstatus);
}

/**
  * @brief  Selects od Deselects the corresponding card.
  * @param  addr: Address of the Card to be selected.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_SelectDeselect(uint32_t addr)
{
  EMMC_Error errorstatus = EMMC_OK;

  /*!< Send CMD7 SDIO_SEL_DESEL_CARD */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument =  addr;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SEL_DESEL_CARD;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SEL_DESEL_CARD);

  return(errorstatus);
}


EMMC_Error EMMC_ReadExtCsd(EMMC_CardInfo *E)
{
    EMMC_Error Result = EMMC_OK;
    uint32_t count = 0;
    uint32_t *ExtCsdBuf;
    ExtCsdBuf = (uint32_t *)(&(E->EMMC_ExtCsd.CsdBuf[0]));

   

    EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
    EMMC_SDIO_DataInitStructure.SDIO_DataLength = (uint32_t)512;
    EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
    EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);

    /* CMD8 */ 
    EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0;
    EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_HS_SEND_EXT_CSD;
    EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

    Result = EMMC_CmdResp1Error(EMMC_CMD_HS_SEND_EXT_CSD);

    if (EMMC_OK != Result)
    {
        return(Result);
    }


    while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
        {
            for (count = 0; count < 8; count++)
            {
                *(ExtCsdBuf + count) = SDIO_ReadData();
            }
            ExtCsdBuf += 8;
        }
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        Result = EMMC_DATA_TIMEOUT;
        return(Result);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        Result = EMMC_DATA_CRC_FAIL;
        return(Result);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        Result =EMMC_RX_OVERRUN;
        return(Result);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        Result = EMMC_START_BIT_ERR;
        return(Result);
    }
    count = EMMC_DATATIMEOUT;
        
    while ((SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) && (count > 0))
    {
        *ExtCsdBuf = SDIO_ReadData();
        ExtCsdBuf++;
        count--;
    }
    
    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        
    return Result;
}

/**
  * @brief  Allows to read one block from a specified address in a card. The Data
  *         transfer can be managed by DMA mode or Polling mode. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.            
  * @param  readbuff: pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data are to be read.  
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_ReadBlock(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize)
{
  EMMC_Error errorstatus = EMMC_OK;
#if defined (SD_POLLING_MODE) 
  uint32_t count = 0, *tempbuff = (uint32_t *)readbuff;
#endif

  EMMC_TransferError = EMMC_OK;
  EMMC_TransferEnd = 0;
  EMMC_StopCondition = 0;
  
  SDIO->DCTRL = 0x0;

  /* Set Block Size for Card */ 
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SET_BLOCKLEN;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SET_BLOCKLEN);

  if (EMMC_OK != errorstatus)
  {
    return(errorstatus);
  }
  
  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);
	
	EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);
	
	

  /*!< Send CMD17 READ_SINGLE_BLOCK */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_READ_SINGLE_BLOCK;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_READ_SINGLE_BLOCK);

  if (errorstatus != EMMC_OK)
  {
    return(errorstatus);
  }

#if defined (SD_POLLING_MODE)  
  /*!< In case of single block transfer, no need of stop transfer at all.*/
  /*!< Polling mode */
  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(tempbuff + count) = SDIO_ReadData();
      }
      tempbuff += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = EMMC_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = EMMC_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus =EMMC_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = EMMC_START_BIT_ERR;
    return(errorstatus);
  }
  count = EMMC_DATATIMEOUT;
  while ((SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) && (count > 0))
  {
    *tempbuff = SDIO_ReadData();
    tempbuff++;
    count--;
  }
  
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
#endif

#if defined (SD_DMA_MODE)
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
    SDIO_DMACmd(ENABLE);
    EMMC_LowLevelDMARxConfig((uint32_t *)readbuff, BlockSize);
	
		errorstatus = EMMC_WaitReadOperation();
		if(errorstatus !=  EMMC_OK)
		{
			return errorstatus;
		}
	
#endif
  return(errorstatus);
}

/**
  * @brief  Allows to read blocks from a specified address  in a card.  The Data
  *         transfer can be managed by DMA mode or Polling mode. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - EMMC_GetStatus(): to check that the SD Card has finished the 
  *            data transfmer and it is ready for data.   
  * @param  readbuff: pointer to the buffer that will contain the received data.
  * @param  ReadAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_ReadMultiBlocks(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  EMMC_Error errorstatus = EMMC_OK;
#if defined (SD_POLLING_MODE) 
  uint32_t count = 0, *tempbuff = (uint32_t *)readbuff;
#endif

  EMMC_TransferError = EMMC_OK;
  EMMC_TransferEnd = 0;
  EMMC_StopCondition = 1;

  CPU_SR_ALLOC();    
  OS_CRITICAL_ENTER();

  SDIO->DCTRL = 0x0;

  /*!< Set Block Size for Card */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SET_BLOCKLEN;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SET_BLOCKLEN);

  if (EMMC_OK != errorstatus)
  {
    return(errorstatus);
  }
    
  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);
	
  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);

  /*!< Send CMD18 READ_MULT_BLOCK with argument data address */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_READ_MULT_BLOCK;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_READ_MULT_BLOCK);
  OS_CRITICAL_EXIT();
  if (errorstatus != EMMC_OK)
  {
    return(errorstatus);
  }

#if defined (SD_POLLING_MODE)  

   while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DATAEND | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
   {
     if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
     {
	   for (count = 0; count < EMMC_HALFFIFO; count++)
       {
         *(tempbuff + count) = SDIO_ReadData();
       }
       tempbuff += EMMC_HALFFIFO;
     }
   }

   if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
   {
     SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
     errorstatus = EMMC_DATA_TIMEOUT;
     return(errorstatus);
   }
   else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
   {
     SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
     errorstatus = EMMC_DATA_CRC_FAIL;
     return(errorstatus);
   }
   else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
   {
     SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
     errorstatus = EMMC_RX_OVERRUN;
     return(errorstatus);
   }
   else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
   {
     SDIO_ClearFlag(SDIO_FLAG_STBITERR);
     errorstatus = EMMC_START_BIT_ERR;
     return(errorstatus);
   }
   while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
   {
     *tempbuff = SDIO_ReadData();
     tempbuff++;
   }

   if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
   {
     /* In Case Of SD-CARD Send Command STOP_TRANSMISSION */
//     if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType))
//     {
       /* Send CMD12 STOP_TRANSMISSION */
       EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0x0;
       EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_STOP_TRANSMISSION;
       EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
       EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
       EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
       SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

       errorstatus = EMMC_CmdResp1Error(EMMC_CMD_STOP_TRANSMISSION);

       if (errorstatus != EMMC_OK)
       {
         return(errorstatus);
       }
//     }
   }   
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
#endif

#if defined (SD_DMA_MODE)
    OS_CRITICAL_ENTER();
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
    EMMC_LowLevelDMARxConfig((uint32_t *)readbuff, (NumberOfBlocks * BlockSize));   
    SDIO_DMACmd(ENABLE);
    OS_CRITICAL_EXIT();
   
    errorstatus = EMMC_WaitReadOperation();
    if(errorstatus !=  EMMC_OK) {
        return errorstatus;
    }
#endif			
  return(errorstatus);
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  *         This function should be called after SDIO_ReadMultiBlocks() function
  *         to insure that all data sent by the card are already transferred by 
  *         the DMA controller.
  * @param  None.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_WaitReadOperation(void)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t timeout;
	

  timeout = EMMC_DATATIMEOUT;
  CPU_SR_ALLOC();
  while ((EMMC_DMAEndOfTransfer == 0x00) && (EMMC_TransferEnd == 0) && (EMMC_TransferError == EMMC_OK) && (timeout > 0))
  {
    timeout--;
  }
  
  EMMC_DMAEndOfTransfer = 0x00;

  timeout = EMMC_DATATIMEOUT;
  
  while(((SDIO->STA & SDIO_FLAG_RXACT)) && (timeout > 0))
  {
    timeout--;  
  }

  if (EMMC_StopCondition == 1)
  {
		OS_CRITICAL_ENTER();
    errorstatus = EMMC_StopTransfer();
		OS_CRITICAL_EXIT();
		EMMC_StopCondition = 0;
  }
  
  if ((timeout == 0) && (errorstatus == EMMC_OK))
  {
    errorstatus = EMMC_DATA_TIMEOUT;
  }
  
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  if (EMMC_TransferError != EMMC_OK)
  {
    return(EMMC_TransferError);
  }
  else
  {
    return(errorstatus);  
  }
}

/**
  * @brief  Allows to write one block starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode or Polling mode.
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.      
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  WriteAddr: Address from where data are to be read.   
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_WriteBlock(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize)
{

  EMMC_Error errorstatus = EMMC_OK;
	EMMCTransferState TranResult;
	uint8_t cardstate = 0;

#if defined (SD_POLLING_MODE)
  uint32_t bytestransferred = 0, count = 0, restwords = 0;
  uint32_t *tempbuff = (uint32_t *)writebuff;
#endif

  EMMC_TransferError = EMMC_OK;
  EMMC_TransferEnd = 0;
  EMMC_StopCondition = 0;
  
				
  SDIO->DCTRL = 0x0;

  /* Set Block Size for Card */ 
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SET_BLOCKLEN;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SET_BLOCKLEN);

  if (EMMC_OK != errorstatus)
  {
    return(errorstatus);
  }
    
  /*!< Send CMD24 WRITE_SINGLE_BLOCK */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = WriteAddr;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_WRITE_SINGLE_BLOCK;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_WRITE_SINGLE_BLOCK);

  if (errorstatus != EMMC_OK)
  {
    return(errorstatus);
  }

  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);
	
  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);

	do
	{		
	    TranResult = EMMC_GetStatus();
	}while(TranResult != EMMC_TRANSFER_OK);
	
  /*!< In case of single data block transfer no need of stop command at all */
#if defined (SD_POLLING_MODE) 
  while (!(SDIO->STA & (SDIO_FLAG_DBCKEND | SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
    {
      if ((512 - bytestransferred) < 32)
      {
        restwords = ((512 - bytestransferred) % 4 == 0) ? ((512 - bytestransferred) / 4) : (( 512 -  bytestransferred) / 4 + 1);
        for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
        {
          SDIO_WriteData(*tempbuff);
        }
      }
      else
      {
        for (count = 0; count < 8; count++)
        {
          SDIO_WriteData(*(tempbuff + count));
        }
        tempbuff += 8;
        bytestransferred += 32;
      }
    }
  }
  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = EMMC_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = EMMC_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
    errorstatus = EMMC_TX_UNDERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = EMMC_START_BIT_ERR;
    return(errorstatus);
  }
#endif
#if defined (SD_DMA_MODE)
	
  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
  EMMC_LowLevelDMATxConfig((uint32_t *)writebuff, BlockSize);
  SDIO_DMACmd(ENABLE);

	errorstatus = EMMC_WaitWriteOperation();
	if(errorstatus !=  EMMC_OK)
		{
			return errorstatus;
		}
	
	errorstatus = EMMC_IsCardProgramming(&cardstate);

  while ((errorstatus == EMMC_OK) && ((EMMC_CARD_PROGRAMMING == cardstate) || (EMMC_CARD_RECEIVING == cardstate)))
  {

    errorstatus = EMMC_IsCardProgramming(&cardstate);
	
  }
	
#endif
  return(errorstatus);
}

/**
  * @brief  Allows to write blocks starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode only. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.     
  * @param  WriteAddr: Address from where data are to be read.
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_WriteMultiBlocks(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  EMMC_Error errorstatus = EMMC_OK;
	uint32_t i = 0;
	EMMCTransferState TranResult;
	uint8_t cardstate = 0;
  CPU_SR_ALLOC();
  EMMC_TransferError = EMMC_OK;
  EMMC_TransferEnd = 0;
//  EMMC_StopCondition = 1;
  
  SDIO->DCTRL = 0x0;

	OS_CRITICAL_ENTER();
	
  /* Set Block Size for Card */ 
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SET_BLOCKLEN;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SET_BLOCKLEN);

  if (EMMC_OK != errorstatus)
  {
    return(errorstatus);
  }
  

  /*!< To improve performance */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) NumberOfBlocks;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SET_BLOCK_COUNT;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SET_BLOCK_COUNT);

  if (errorstatus != EMMC_OK)
  {
    return(errorstatus);
  }


  /*!< Send CMD25 WRITE_MULT_BLOCK with argument data address */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)WriteAddr;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_WRITE_MULT_BLOCK;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_WRITE_MULT_BLOCK);

  if (EMMC_OK != errorstatus)
  {
    return(errorstatus);
  }

  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);
	
  EMMC_SDIO_DataInitStructure.SDIO_DataTimeOut = EMMC_DATATIMEOUT;
  EMMC_SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  EMMC_SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  EMMC_SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  EMMC_SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  EMMC_SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&EMMC_SDIO_DataInitStructure);
  OS_CRITICAL_EXIT();
	do
	{			
			OS_CRITICAL_ENTER();		
			TranResult = EMMC_GetStatus();
			OS_CRITICAL_EXIT();
	}while(TranResult != EMMC_TRANSFER_OK);
	OS_CRITICAL_ENTER();
  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
  SDIO_DMACmd(ENABLE);
	EMMC_LowLevelDMATxConfig((uint32_t *)writebuff, (NumberOfBlocks * BlockSize));
  OS_CRITICAL_EXIT();
	

	errorstatus = EMMC_WaitWriteOperation();

	if (EMMC_OK != errorstatus)
  {
    return(errorstatus);
  }
		OS_CRITICAL_ENTER();
	 errorstatus = EMMC_IsCardProgramming(&cardstate);
	  OS_CRITICAL_EXIT();
	while ((errorstatus == EMMC_OK) && ((EMMC_CARD_PROGRAMMING == cardstate) || (EMMC_CARD_RECEIVING == cardstate)))
  {
		OS_CRITICAL_ENTER();
    errorstatus = EMMC_IsCardProgramming(&cardstate);
		OS_CRITICAL_EXIT();
		for(i=0; i<10000; i++);
  }
  return(errorstatus);
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  *         This function should be called after SDIO_WriteBlock() and
  *         SDIO_WriteMultiBlocks() function to insure that all data sent by the 
  *         card are already transferred by the DMA controller.        
  * @param  None.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_WaitWriteOperation(void)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t timeout;

  timeout = EMMC_DATATIMEOUT;
  
  while ((EMMC_DMAEndOfTransfer == 0x00) && (EMMC_TransferEnd == 0) && (EMMC_TransferError == EMMC_OK) && (timeout > 0))
  {
    timeout--;
  }
  
  EMMC_DMAEndOfTransfer = 0x00;

  timeout = EMMC_DATATIMEOUT;
	
//	if (EMMC_StopCondition == 1)
//	{
//		errorstatus = EMMC_StopTransfer();
//		EMMC_StopCondition = 0;
//	}
	
  while(((SDIO->STA & SDIO_FLAG_TXACT)) && (timeout > 0))
  {
    timeout--;  
  }
	/*!< Clear all the static flags */
		SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  if ((timeout == 0) && (errorstatus == EMMC_OK))
  {
    errorstatus = EMMC_DATA_TIMEOUT;
  }
  if (EMMC_TransferError != EMMC_OK)
  {
    return(EMMC_TransferError);
  }
  else
  {
    return(errorstatus);
  }
}

/**
  * @brief  Gets the cuurent data transfer state.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
EMMCTransferState EMMC_GetTransferState(void)
{
  if (SDIO->STA & (SDIO_FLAG_TXACT | SDIO_FLAG_RXACT))
  {
    return(EMMC_TRANSFER_BUSY);
  }
  else
  {
    return(EMMC_TRANSFER_OK);
  }
}

/**
  * @brief  Aborts an ongoing data transfer.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_StopTransfer(void)
{
  EMMC_Error errorstatus = EMMC_OK;

  /*!< Send CMD12 STOP_TRANSMISSION  */
  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)0;//(MyEMMC_CardInfo.RCA << 16);
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_STOP_TRANSMISSION;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_STOP_TRANSMISSION);
  return(errorstatus);
}

/**
  * @brief  Allows to erase memory area specified for the given card.
  * @param  startaddr: the start address.
  * @param  endaddr: the end address.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_Erase(uint32_t startaddr, uint32_t endaddr)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t delay = 0;
  __IO uint32_t maxdelay = 0;
  uint8_t cardstate = 0;
	
	/*!< Check if the card coomnd class supports erase command */
//  if (((CSD_Tab[1] >> 20) & SD_CCCC_ERASE) == 0)
		if ((MyEMMC_CardInfo.EMMC_Csd.CCC & EMMC_CCCC_ERASE) == 0)
		{
			errorstatus = EMMC_REQUEST_NOT_APPLICABLE;
			return(errorstatus);
		}

		maxdelay = 9200000 / ((SDIO->CLKCR & 0xFF) + 2);

		if (SDIO_GetResponse(SDIO_RESP1) & EMMC_CARD_LOCKED)
		{
			errorstatus = EMMC_LOCK_UNLOCK_FAILED;
			return(errorstatus);
		}

     /*!< Send CMD35 SD_ERASE_GRP_START with argument as addr  */
     EMMC_SDIO_CmdInitStructure.SDIO_Argument = startaddr;
     EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_ERASE_GRP_START;
     EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
     EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
     EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
     SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

     errorstatus = EMMC_CmdResp1Error(EMMC_CMD_ERASE_GRP_START);
     if (errorstatus != EMMC_OK)
     {
       return(errorstatus);
     }
//      for (delay = 0; delay < 10000; delay++);
     /*!< Send CMD36 SD_ERASE_GRP_END with argument as addr  */
     EMMC_SDIO_CmdInitStructure.SDIO_Argument = endaddr;
     EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_ERASE_GRP_END;
     EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
     EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
     EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
     SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

     errorstatus = EMMC_CmdResp1Error(EMMC_CMD_ERASE_GRP_END);
     if (errorstatus != EMMC_OK)
     {
       return(errorstatus);
     }
//		  for (delay = 0; delay < 10000; delay++);
   /*!< Send CMD38 ERASE */
//   EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0x03;
	EMMC_SDIO_CmdInitStructure.SDIO_Argument = 0x00;
   EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_ERASE;
   EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
   EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
   EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
   SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

   errorstatus = EMMC_CmdResp1Error(EMMC_CMD_ERASE);

   if (errorstatus != EMMC_OK)
   {
     return(errorstatus);
   }
  for (delay = 0; delay < maxdelay; delay++)
  {}

  /*!< Wait till the card is in programming state */
  errorstatus = EMMC_IsCardProgramming(&cardstate);
  delay = EMMC_DATATIMEOUT;
  while ((delay > 0) && (errorstatus == EMMC_OK) && ((EMMC_CARD_PROGRAMMING == cardstate) || (EMMC_CARD_RECEIVING == cardstate)))
  {
    errorstatus = EMMC_IsCardProgramming(&cardstate);
    delay--;
  }

  return(errorstatus);
}

/**
  * @brief  Returns the current card's status.
  * @param  pcardstatus: pointer to the buffer that will contain the SD card 
  *         status (Card Status register).
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_SendStatus(uint32_t *pcardstatus)
{
  EMMC_Error errorstatus = EMMC_OK;

  if (pcardstatus == NULL)
  {
    errorstatus = EMMC_INVALID_PARAMETER;
    return(errorstatus);
  }

  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) MyEMMC_CardInfo.RCA << 16;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SEND_STATUS;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  errorstatus = EMMC_CmdResp1Error(EMMC_CMD_SEND_STATUS);

  if (errorstatus != EMMC_OK)
  {
    return(errorstatus);
  }

  *pcardstatus = SDIO_GetResponse(SDIO_RESP1);

  return(errorstatus);
}

/**
  * @brief  Allows to process all the interrupts that are high.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
//EMMC_Error EMMC_ProcessIRQSrc(void)
//{ 
//  if (SDIO_GetITStatus(SDIO_IT_DATAEND) != RESET)
//  {
//    EMMC_TransferError = EMMC_OK;
//    SDIO_ClearITPendingBit(SDIO_IT_DATAEND);
//    EMMC_TransferEnd = 1;
//  }  
//  else if (SDIO_GetITStatus(SDIO_IT_DCRCFAIL) != RESET)
//  {
//    SDIO_ClearITPendingBit(SDIO_IT_DCRCFAIL);
//    EMMC_TransferError = EMMC_DATA_CRC_FAIL;
//  }
//  else if (SDIO_GetITStatus(SDIO_IT_DTIMEOUT) != RESET)
//  {
//    SDIO_ClearITPendingBit(SDIO_IT_DTIMEOUT);
//    EMMC_TransferError = EMMC_DATA_TIMEOUT;
//  }
//  else if (SDIO_GetITStatus(SDIO_IT_RXOVERR) != RESET)
//  {
//    SDIO_ClearITPendingBit(SDIO_IT_RXOVERR);
//    EMMC_TransferError = EMMC_RX_OVERRUN;
//  }
//  else if (SDIO_GetITStatus(SDIO_IT_TXUNDERR) != RESET)
//  {
//    SDIO_ClearITPendingBit(SDIO_IT_TXUNDERR);
//    EMMC_TransferError = EMMC_TX_UNDERRUN;
//  }
//  else if (SDIO_GetITStatus(SDIO_IT_STBITERR) != RESET)
//  {
//    SDIO_ClearITPendingBit(SDIO_IT_STBITERR);
//    EMMC_TransferError = EMMC_START_BIT_ERR;
//  }

//  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
//                SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
//                SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
//  return(EMMC_TransferError);
//}

///**
//  * @brief  This function waits until the SDIO DMA data transfer is finished. 
//  * @param  None.
//  * @retval None.
//  */
//void EMMC_ProcessDMAIRQ(void)
//{
//  if(DMA2->LISR & EMMC_SDIO_DMA_FLAG_TCIF)
//  {
//    EMMC_DMAEndOfTransfer = 0x01;
//    DMA_ClearFlag(EMMC_SDIO_DMA_STREAM, EMMC_SDIO_DMA_FLAG_TCIF|EMMC_SDIO_DMA_FLAG_FEIF);
//  }
//}

/**
  * @brief  Checks for error conditions for CMD0.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
static EMMC_Error EMMC_CmdError(void)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t timeout;

  timeout = SDIO_CMD0TIMEOUT; /*!< 10000 */

  while ((timeout > 0) && (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET))
  {
    timeout--;
  }

  if (timeout == 0)
  {
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/**
  * @brief  Checks for error conditions for R7 response.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_CmdResp7Error(void)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t status;
  uint32_t timeout = SDIO_CMD0TIMEOUT;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) && (timeout > 0))
  {
    timeout--;
    status = SDIO->STA;
  }

  if ((timeout == 0) || (status & SDIO_FLAG_CTIMEOUT))
  {
    /*!< Card is not V2.0 complient or card does not support the set voltage range */
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }

  if (status & SDIO_FLAG_CMDREND)
  {
    /*!< Card is SD V2.0 compliant */
    errorstatus = EMMC_OK;
    SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    return(errorstatus);
  }
  return(errorstatus);
}

/**
  * @brief  Checks for error conditions for R1 response.
  * @param  cmd: The sent command index.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_CmdResp1Error(uint8_t cmd)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t status;
  uint32_t response_r1;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = EMMC_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Check response received is of desired command */
  if (SDIO_GetCommandResponse() != cmd)
  {
    errorstatus = EMMC_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /*!< We have received response, retrieve it for analysis  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);

  if ((response_r1 & EMMC_OCR_ERRORBITS) == EMMC_ALLZERO)
  {
    return(errorstatus);
  }

  if (response_r1 & EMMC_OCR_ADDR_OUT_OF_RANGE)
  {
    return(EMMC_ADDR_OUT_OF_RANGE);
  }

  if (response_r1 & EMMC_OCR_ADDR_MISALIGNED)
  {
    return(EMMC_ADDR_MISALIGNED);
  }

  if (response_r1 & EMMC_OCR_BLOCK_LEN_ERR)
  {
    return(EMMC_BLOCK_LEN_ERR);
  }

  if (response_r1 & EMMC_OCR_ERASE_SEQ_ERR)
  {
    return(EMMC_ERASE_SEQ_ERR);
  }

  if (response_r1 & EMMC_OCR_BAD_ERASE_PARAM)
  {
    return(EMMC_BAD_ERASE_PARAM);
  }

  if (response_r1 & EMMC_OCR_WRITE_PROT_VIOLATION)
  {
    return(EMMC_WRITE_PROT_VIOLATION);
  }

  if (response_r1 & EMMC_OCR_LOCK_UNLOCK_FAILED)
  {
    return(EMMC_LOCK_UNLOCK_FAILED);
  }

  if (response_r1 & EMMC_OCR_COM_CRC_FAILED)
  {
    return(EMMC_COM_CRC_FAILED);
  }

  if (response_r1 & EMMC_OCR_ILLEGAL_CMD)
  {
    return(EMMC_ILLEGAL_CMD);
  }

  if (response_r1 & EMMC_OCR_CARD_ECC_FAILED)
  {
    return(EMMC_CARD_ECC_FAILED);
  }

  if (response_r1 & EMMC_OCR_CC_ERROR)
  {
    return(EMMC_CC_ERROR);
  }

  if (response_r1 & EMMC_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(EMMC_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & EMMC_OCR_STREAM_READ_UNDERRUN)
  {
    return(EMMC_STREAM_READ_UNDERRUN);
  }

  if (response_r1 & EMMC_OCR_STREAM_WRITE_OVERRUN)
  {
    return(EMMC_STREAM_WRITE_OVERRUN);
  }

//   if (response_r1 & EMMC_OCR_CID_CSD_OVERWRIETE)
//   {
//   //  return(EMMC_CID_CSD_OVERWRITE);
//   }

//   if (response_r1 & EMMC_OCR_WP_ERASE_SKIP)
//   {
//     return(EMMC_WP_ERASE_SKIP);
//   }

  if (response_r1 & EMMC_OCR_CARD_ECC_DISABLED)
  {
    return(EMMC_CARD_ECC_DISABLED);
  }

  if (response_r1 & EMMC_OCR_ERASE_RESET)
  {
    return(EMMC_ERASE_RESET);
  }

  if (response_r1 & EMMC_OCR_AKE_SEQ_ERROR)
  {
    return(EMMC_AKE_SEQ_ERROR);
  }
  return(errorstatus);
}

/**
  * @brief  Checks for error conditions for R3 (OCR) response.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_CmdResp3Error(void)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t status;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return(errorstatus);
}

/**
  * @brief  Checks for error conditions for R2 (CID or CSD) response.
  * @param  None
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_CmdResp2Error(void)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t status;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = EMMC_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/**
  * @brief  Checks for error conditions for R6 (RCA) response.
  * @param  cmd: The sent command index.
  * @param  prca: pointer to the variable that will contain the SD card relative 
  *         address RCA. 
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_CmdResp6Error(uint8_t cmd, uint16_t *prca)
{
  EMMC_Error errorstatus = EMMC_OK;
  uint32_t status;
  uint32_t response_r1;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = EMMC_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Check response received is of desired command */
  if (SDIO_GetCommandResponse() != cmd)
  {
    errorstatus = EMMC_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /*!< We have received response, retrieve it.  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);

  if (EMMC_ALLZERO == (response_r1 & (EMMC_R6_GENERAL_UNKNOWN_ERROR | EMMC_R6_ILLEGAL_CMD | EMMC_R6_COM_CRC_FAILED)))
  {
    *prca = (uint16_t) (response_r1 >> 16);
    return(errorstatus);
  }

  if (response_r1 & EMMC_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(EMMC_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & EMMC_R6_ILLEGAL_CMD)
  {
    return(EMMC_ILLEGAL_CMD);
  }

  if (response_r1 & EMMC_R6_COM_CRC_FAILED)
  {
    return(EMMC_COM_CRC_FAILED);
  }

  return(errorstatus);
}

/**
  * @brief  Enables or disables the SDIO wide bus mode.
  * @param  NewState: new state of the SDIO wide bus mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_EnWideBus(FunctionalState NewState)
{
  EMMC_Error Result = EMMC_ERROR;

//  uint32_t scr[2] = {0, 0};

  if (SDIO_GetResponse(SDIO_RESP1) & EMMC_CARD_LOCKED)
  {
    Result = EMMC_LOCK_UNLOCK_FAILED;
    return(Result);
  }

  /*!< If wide bus operation to be enabled */
  if (NewState == ENABLE)
  {

				/* CMD6 */ 
			EMMC_SDIO_CmdInitStructure.SDIO_Argument = EMMC_POWER_REG;
			EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_HS_SWITCH;
			EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

			Result = EMMC_CmdResp1Error(EMMC_CMD_HS_SWITCH);
		  if (EMMC_OK != Result)
			{
				return(Result);
			}
			
            drv_delay_ms(100);
							/* CMD6 */ 
			EMMC_SDIO_CmdInitStructure.SDIO_Argument = EMMC_HIGHSPEED_REG;
			EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_HS_SWITCH;
			EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

			Result = EMMC_CmdResp1Error(EMMC_CMD_HS_SWITCH);
		  if (EMMC_OK != Result)
			{
				return(Result);
			}
			drv_delay_ms(100);
			/* CMD6 */ 
			EMMC_SDIO_CmdInitStructure.SDIO_Argument = EMMC_4BIT_REG;
			EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_HS_SWITCH;
			EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

			Result = EMMC_CmdResp1Error(EMMC_CMD_HS_SWITCH);
			
			if (EMMC_OK != Result)
			{
				return(Result);
			}
      return(Result);
  }   /*!< If wide bus operation to be disabled */
  else
  {
      Result = EMMC_CmdResp1Error(EMMC_CMD_APP_CMD);

      if (Result != EMMC_OK)
      {
        return(Result);
      }

      if (Result != EMMC_OK)
      {
        return(Result);
      }

      return(Result);
  }
}

/**
  * @brief  Checks if the SD card is in programming state.
  * @param  pstatus: pointer to the variable that will contain the SD card state.
  * @retval EMMC_Error: SD Card Error code.
  */
EMMC_Error EMMC_IsCardProgramming(uint8_t *pstatus)
{
  EMMC_Error errorstatus = EMMC_OK;
  __IO uint32_t respR1 = 0, status = 0;

  EMMC_SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) MyEMMC_CardInfo.RCA << 16;
  EMMC_SDIO_CmdInitStructure.SDIO_CmdIndex = EMMC_CMD_SEND_STATUS;
  EMMC_SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  EMMC_SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  EMMC_SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&EMMC_SDIO_CmdInitStructure);

  status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = EMMC_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = EMMC_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  status = (uint32_t)SDIO_GetCommandResponse();

  /*!< Check response received is of desired command */
  if (status != EMMC_CMD_SEND_STATUS)
  {
    errorstatus = EMMC_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);


  /*!< We have received response, retrieve it for analysis  */
  respR1 = SDIO_GetResponse(SDIO_RESP1);

  /*!< Find out card status */
  *pstatus = (uint8_t) ((respR1 >> 9) & 0x0000000F);

  if ((respR1 & EMMC_OCR_ERRORBITS) == EMMC_ALLZERO)
  {
    return(errorstatus);
  }

  if (respR1 & EMMC_OCR_ADDR_OUT_OF_RANGE)
  {
    return(EMMC_ADDR_OUT_OF_RANGE);
  }

  if (respR1 & EMMC_OCR_ADDR_MISALIGNED)
  {
    return(EMMC_ADDR_MISALIGNED);
  }

  if (respR1 & EMMC_OCR_BLOCK_LEN_ERR)
  {
    return(EMMC_BLOCK_LEN_ERR);
  }

  if (respR1 & EMMC_OCR_ERASE_SEQ_ERR)
  {
    return(EMMC_ERASE_SEQ_ERR);
  }

  if (respR1 & EMMC_OCR_BAD_ERASE_PARAM)
  {
    return(EMMC_BAD_ERASE_PARAM);
  }

  if (respR1 & EMMC_OCR_WRITE_PROT_VIOLATION)
  {
    return(EMMC_WRITE_PROT_VIOLATION);
  }

  if (respR1 & EMMC_OCR_LOCK_UNLOCK_FAILED)
  {
    return(EMMC_LOCK_UNLOCK_FAILED);
  }

  if (respR1 & EMMC_OCR_COM_CRC_FAILED)
  {
    return(EMMC_COM_CRC_FAILED);
  }

  if (respR1 & EMMC_OCR_ILLEGAL_CMD)
  {
    return(EMMC_ILLEGAL_CMD);
  }

  if (respR1 & EMMC_OCR_CARD_ECC_FAILED)
  {
    return(EMMC_CARD_ECC_FAILED);
  }

  if (respR1 & EMMC_OCR_CC_ERROR)
  {
    return(EMMC_CC_ERROR);
  }

  if (respR1 & EMMC_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(EMMC_GENERAL_UNKNOWN_ERROR);
  }

  if (respR1 & EMMC_OCR_STREAM_READ_UNDERRUN)
  {
    return(EMMC_STREAM_READ_UNDERRUN);
  }

  if (respR1 & EMMC_OCR_STREAM_WRITE_OVERRUN)
  {
    return(EMMC_STREAM_WRITE_OVERRUN);
  }

//   if (respR1 & EMMC_OCR_CID_CSD_OVERWRIETE)
//   {
//     return(EMMC_CID_CSD_OVERWRITE);
//   }

//   if (respR1 & EMMC_OCR_WP_ERASE_SKIP)
//   {
//     return(EMMC_WP_ERASE_SKIP);
//   }

  if (respR1 & EMMC_OCR_CARD_ECC_DISABLED)
  {
    return(EMMC_CARD_ECC_DISABLED);
  }

  if (respR1 & EMMC_OCR_ERASE_RESET)
  {
    return(EMMC_ERASE_RESET);
  }

  if (respR1 & EMMC_OCR_AKE_SEQ_ERROR)
  {
    return(EMMC_AKE_SEQ_ERROR);
  }

  return(errorstatus);
}


/**
  * @brief  Converts the number of bytes in power of two and returns the power.
  * @param  NumberOfBytes: number of bytes.
  * @retval None
  */
uint8_t emmc_convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
  uint8_t count = 0;

  while (NumberOfBytes != 1)
  {
    NumberOfBytes >>= 1;
    count++;
  }
  return(count);
}


EMMC_Error EMMC_ProcessIRQSrc(void)
{ 
  if (SDIO_GetITStatus(SDIO_IT_DATAEND) != RESET)
  {
    EMMC_TransferError = EMMC_OK;
    SDIO_ClearITPendingBit(SDIO_IT_DATAEND);
    EMMC_TransferEnd = 1;
  }  
  else if (SDIO_GetITStatus(SDIO_IT_DCRCFAIL) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_DCRCFAIL);
    EMMC_TransferError = EMMC_DATA_CRC_FAIL;
  }
  else if (SDIO_GetITStatus(SDIO_IT_DTIMEOUT) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_DTIMEOUT);
    EMMC_TransferError = EMMC_DATA_TIMEOUT;
  }
  else if (SDIO_GetITStatus(SDIO_IT_RXOVERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_RXOVERR);
    EMMC_TransferError = EMMC_RX_OVERRUN;
  }
  else if (SDIO_GetITStatus(SDIO_IT_TXUNDERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_TXUNDERR);
    EMMC_TransferError = EMMC_TX_UNDERRUN;
  }
  else if (SDIO_GetITStatus(SDIO_IT_STBITERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_STBITERR);
    EMMC_TransferError = EMMC_START_BIT_ERR;
  }

  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
  return(EMMC_TransferError);
}

void EMMC_ProcessDMAIRQ(void)
{
  if(DMA2->LISR & EMMC_SDIO_DMA_FLAG_TCIF)
  {
    EMMC_DMAEndOfTransfer = 0x01;
    DMA_ClearFlag(EMMC_SDIO_DMA_STREAM, EMMC_SDIO_DMA_FLAG_TCIF|EMMC_SDIO_DMA_FLAG_FEIF);
  }
}

/******************************中断服务函数*************************************/
/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
void EMMC_SDIO_IRQHandler(void)
{
    EMMC_ProcessIRQSrc();
}

/**
  * @brief  This function handles DMA2 Stream3 or DMA2 Stream6 global interrupts
  *         requests.
  * @param  None
  * @retval None
  */


void EMMC_SDIO_DMA_IRQHANDLER(void)
{
    EMMC_ProcessDMAIRQ();
}


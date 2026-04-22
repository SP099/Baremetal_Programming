/*
 * stm32f407xx_spi_driver.h
 *
 *  Created on: Apr 20, 2026
 *      Author: Lenovo
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"

/*********************************************************************
 * @brief  Configuration structure for SPI peripheral
 *
 * @details
 *  This structure is used to configure the SPI peripheral.
 *  User fills this structure and passes it to SPI_Init().
 *
 *  It contains all important SPI configuration parameters such as:
 *   - Master/Slave mode
 *   - Bus configuration (Full duplex / Half duplex / Simplex)
 *   - Clock speed
 *   - Data frame format
 *   - Clock polarity and phase
 *   - Software slave management
 *
 *********************************************************************/
typedef struct
{
    uint8_t SPI_DeviceMode;   /*
                               * Specifies device mode:
                               *   - SPI_DEVICE_MODE_MASTER
                               *   - SPI_DEVICE_MODE_SLAVE
                               */

    uint8_t SPI_BusConfig;    /*
                               * Specifies bus configuration:
                               *   - SPI_BUS_CONFIG_FD  (Full Duplex)
                               *   - SPI_BUS_CONFIG_HD  (Half Duplex)
                               *   - SPI_BUS_CONFIG_SIMPLEX_RXONLY
                               */

    uint8_t SPI_SclkSpeed;    /*
                               * Specifies SPI clock speed (baud rate):
                               *   - SPI_SCLK_SPEED_DIV2
                               *   - SPI_SCLK_SPEED_DIV4
                               *   - ... up to DIV256
                               *
                               * This sets BR[2:0] bits in CR1 register
                               */

    uint8_t SPI_DFF;          /*
                               * Specifies Data Frame Format:
                               *   - SPI_DFF_8BITS
                               *   - SPI_DFF_16BITS
                               *
                               * Controls DFF bit in CR1 register
                               */

    uint8_t SPI_CPOL;         /*
                               * Specifies Clock Polarity:
                               *   - SPI_CPOL_LOW  → Idle state LOW
                               *   - SPI_CPOL_HIGH → Idle state HIGH
                               *
                               * Controls CPOL bit in CR1
                               */

    uint8_t SPI_CPHA;         /*
                               * Specifies Clock Phase:
                               *   - SPI_CPHA_LOW  → Data sampled on first edge
                               *   - SPI_CPHA_HIGH → Data sampled on second edge
                               *
                               * Controls CPHA bit in CR1
                               */

    uint8_t SPI_SSM;          /*
                               * Specifies Software Slave Management:
                               *   - SPI_SSM_EN  → Software NSS management
                               *   - SPI_SSM_DI  → Hardware NSS management
                               *
                               * Controls SSM bit in CR1
                               *
                               * NOTE:
                               * If SSM = ENABLE → must set SSI = 1
                               * to avoid MODF (Mode Fault)
                               */

} SPI_Config_t;

/*********************************************************************
 * @brief  Handle structure for SPI peripheral
 *
 * @details
 *  This structure is used to manage SPI operations.
 *
 *  It contains:
 *   - Base address of SPI peripheral
 *   - SPI configuration settings
 *   - Transmission and reception buffers
 *   - Length tracking
 *   - Current state of SPI (busy/ready)
 *
 *  This structure is used by:
 *   - Blocking APIs
 *   - Interrupt-based APIs
 *
 *********************************************************************/
typedef struct
{
    SPI_RegDef_t *pSPIx;   /*
                            * Pointer to SPI peripheral base address
                            * Example: SPI1, SPI2, SPI3
                            */

    SPI_Config_t SPIConfig; /*
                             * SPI configuration structure
                             * Contains all user-defined settings
                             */

    uint8_t *pTxBuffer;    /*
                            * Pointer to transmit buffer
                            * Holds data to be sent over SPI
                            */

    uint8_t *pRxBuffer;    /*
                            * Pointer to receive buffer
                            * Stores incoming data from SPI
                            */

    uint8_t TxLen;         /*
                            * Length of data to transmit
                            * Decremented during transmission
                            */

    uint8_t RxLen;         /*
                            * Length of data to receive
                            * Decremented during reception
                            */

    uint8_t TxState;       /*
                            * Transmission state:
                            *   - SPI_READY
                            *   - SPI_BUSY_IN_TX
                            *
                            * Used to prevent overlapping transmissions
                            */

    uint8_t RxState;       /*
                            * Reception state:
                            *   - SPI_READY
                            *   - SPI_BUSY_IN_RX
                            *
                            * Used to prevent overlapping receptions
                            */

} SPI_Handle_t;

/*********************************************************************
 * @SPI_DeviceMode
 *
 * Selects whether SPI operates as MASTER or SLAVE.
 *
 * Hardware mapping:
 *  - SPI_CR1 → MSTR (Bit 2)
 *
 * Values:
 *  - MASTER (1):
 *      → Generates clock (SCLK)
 *      → Controls communication
 *
 *  - SLAVE (0):
 *      → Uses external clock from master
 *
 * WHY macros?
 *  - Improves readability (instead of raw 0/1)
 *  - Directly maps to register bit value
 *********************************************************************/
#define SPI_DEVICE_MODE_MASTER      1
#define SPI_DEVICE_MODE_SLAVE       0

/*********************************************************************
 * @SPI_BusConfig
 *
 * Defines how data lines are used in SPI communication.
 *
 * SPI supports 3 modes:
 *
 * 1. Full Duplex (FD)
 *    → Separate lines for TX and RX
 *    → MOSI + MISO both active
 *
 * 2. Half Duplex (HD)
 *    → Single bidirectional data line
 *
 * 3. Simplex RX Only
 *    → Only receive, no transmission
 *
 * Hardware mapping:
 *  - BIDIMODE (CR1 bit 15)
 *  - RXONLY   (CR1 bit 10)
 *
 * WHY values are 1,2,4?
 *  - Not random
 *  - Allows clean comparison in driver (no confusion with 0 default)
 *  - Avoids accidental overlap
 *********************************************************************/
#define SPI_BUS_CONFIG_FD               1
#define SPI_BUS_CONFIG_HD               2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY   4

/*********************************************************************
 * @SPI_SclkSpeed (Baud Rate Control)
 *
 * Controls SPI clock speed using BR[5:3] bits in SPI_CR1.
 *
 * SPI clock = fPCLK / Prescaler
 *
 * BR bits encoding:
 *   000 → fPCLK/2
 *   001 → fPCLK/4
 *   010 → fPCLK/8
 *   011 → fPCLK/16
 *   100 → fPCLK/32
 *   101 → fPCLK/64
 *   110 → fPCLK/128
 *   111 → fPCLK/256
 *
 * WHY values start from 0?
 *  - Because BR field expects exact binary encoding
 *  - No translation required → direct shift into register
 *
 * Example:
 *   tempreg |= (SPI_SCLK_SPEED_DIV8 << SPI_CR1_BR);
 *********************************************************************/
#define SPI_SCLK_SPEED_DIV2        0
#define SPI_SCLK_SPEED_DIV4        1
#define SPI_SCLK_SPEED_DIV8        2
#define SPI_SCLK_SPEED_DIV16       3
#define SPI_SCLK_SPEED_DIV32       4
#define SPI_SCLK_SPEED_DIV64       5
#define SPI_SCLK_SPEED_DIV128      6
#define SPI_SCLK_SPEED_DIV256      7

/*********************************************************************
 * @SPI_DFF (Data Frame Format)
 *
 * Defines size of data frame.
 *
 * Hardware mapping:
 *  - SPI_CR1 → DFF (Bit 11)
 *
 * Values:
 *  - 0 → 8-bit data
 *  - 1 → 16-bit data
 *
 * WHY important?
 *  - Affects how DR register is accessed
 *  - Must match between master and slave
 *********************************************************************/
#define SPI_DFF_8BITS      0
#define SPI_DFF_16BITS     1

/*********************************************************************
 * @CPOL (Clock Polarity)
 *
 * Defines idle state of SPI clock.
 *
 * Hardware mapping:
 *  - SPI_CR1 → CPOL (Bit 1)
 *
 * Values:
 *  - LOW  (0) → Clock idle = 0
 *  - HIGH (1) → Clock idle = 1
 *
 * WHY needed?
 *  - Must match slave device requirement
 *  - Part of SPI mode (0–3)
 *********************************************************************/
#define SPI_CPOL_LOW   0
#define SPI_CPOL_HIGH  1

/*********************************************************************
 * @CPHA (Clock Phase)
 *
 * Defines when data is sampled.
 *
 * Hardware mapping:
 *  - SPI_CR1 → CPHA (Bit 0)
 *
 * Values:
 *  - LOW  (0) → Sample on first clock edge
 *  - HIGH (1) → Sample on second clock edge
 *
 * WHY important?
 *  - Combined with CPOL → defines SPI Mode
 *
 * SPI Modes:
 *   Mode 0 → CPOL=0, CPHA=0
 *   Mode 1 → CPOL=0, CPHA=1
 *   Mode 2 → CPOL=1, CPHA=0
 *   Mode 3 → CPOL=1, CPHA=1
 *********************************************************************/
#define SPI_CPHA_LOW   0
#define SPI_CPHA_HIGH  1

/*********************************************************************
 * @SPI_SSM (Software Slave Management)
 *
 * Controls NSS (Slave Select) handling.
 *
 * Hardware mapping:
 *  - SPI_CR1 → SSM (Bit 9)
 *  - SPI_CR1 → SSI (Bit 8)
 *
 * Values:
 *  - ENABLE (1):
 *      → NSS managed by software
 *      → Must set SSI = 1 (avoid MODF error)
 *
 *  - DISABLE (0):
 *      → NSS controlled by hardware pin
 *
 * WHY this macro?
 *  - Simplifies NSS control logic
 *  - Avoids direct register manipulation by user
 *********************************************************************/
#define SPI_SSM_EN     1
#define SPI_SSM_DI     0

/*
 * SPI related status flags definition
 * These macros create bit masks for checking specific flags
 */

#define SPI_RXNE_FLAG   (1 << SPI_SR_RXNE)   /* Data received, ready to read */
#define SPI_TXE_FLAG    (1 << SPI_SR_TXE)    /* Ready to transmit new data */
#define SPI_CHSIDE_FLAG (1 << SPI_SR_CHSIDE) /* I2S channel side indicator */
#define SPI_UDR_FLAG    (1 << SPI_SR_UDR)    /* Underrun error */
#define SPI_CRCERR_FLAG (1 << SPI_SR_CRCERR) /* CRC mismatch */
#define SPI_MODF_FLAG   (1 << SPI_SR_MODF)   /* Mode fault */
#define SPI_OVR_FLAG    (1 << SPI_SR_OVR)    /* Overrun error */
#define SPI_BSY_FLAG    (1 << SPI_SR_BSY)    /* SPI is busy transmitting */
#define SPI_FRE_FLAG    (1 << SPI_SR_FRE)    /* Frame error */

/*
 * Possible SPI Applicatiion States
 */
#define SPI_READY				0
#define SPI_BUSY_IN_RX			1
#define SPI_BUSY_IN_TX			2

/*
 * Possible SPI Application events
 */
#define SPI_EVENT_TX_CMPLT   1
#define SPI_EVENT_RX_CMPLT   2
#define SPI_EVENT_OVR_ERR    3
#define SPI_EVENT_CRC_ERR    4

/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

/*
 * Peripheral Clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);
/*
 * IRQ Configuration and ISR handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/*
 * Other Peripheral Control APIs
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint32_t FlagName);
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void  SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmisson(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

/*
 * Application callback
 */
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv);
#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */

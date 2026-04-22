/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Apr 20, 2026
 *      Author: Lenovo
 */

#include "stm32f407xx_spi_driver.h"

/*********************************************************************
 * @fn          - SPI_PeriClockControl
 *
 * @brief       - Enable or disable the peripheral clock for SPIx
 *
 * @param[in]   - pSPIx : Base address of SPI peripheral (SPI1, SPI2, SPI3)
 * @param[in]   - EnorDi: ENABLE or DISABLE macro
 *                        ENABLE  -> Enables SPI peripheral clock
 *                        DISABLE -> Disables SPI peripheral clock
 *
 * @return      - None
 *
 * @details
 *  - Every peripheral in STM32 is connected to a clock bus (APB1/APB2)
 *  - SPI peripherals require clock to be enabled before accessing registers
 *
 *  Clock mapping (STM32F407):
 *    SPI1 → APB2 bus
 *    SPI2 → APB1 bus
 *    SPI3 → APB1 bus
 *
 *  Why this API is needed?
 *  ------------------------------------------------------------
 *  - Registers of a peripheral work only when clock is enabled
 *  - If clock is OFF:
 *        → Register writes have no effect
 *        → Peripheral appears "dead"
 *
 *  So ALWAYS call this before SPI_Init()
 *
 *********************************************************************/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        /*************************************************************
         * =============== ENABLE CLOCK ==============================
         *
         * We compare base address pointer to identify which SPI
         * peripheral is being used.
         *
         * Why pointer comparison?
         *  - SPI1, SPI2, SPI3 are mapped to different base addresses
         *  - So pointer value uniquely identifies the peripheral
         *************************************************************/

        if(pSPIx == SPI1)
        {
            /* Enable clock for SPI1 (APB2ENR register) */
            SPI1_PCLK_EN();
        }
        else if(pSPIx == SPI2)
        {
            /* Enable clock for SPI2 (APB1ENR register) */
            SPI2_PCLK_EN();
        }
        else if(pSPIx == SPI3)
        {
            /* Enable clock for SPI3 (APB1ENR register) */
            SPI3_PCLK_EN();
        }
    }
    else
    {
        /*************************************************************
         * =============== DISABLE CLOCK =============================
         *
         * Disabling clock stops the peripheral operation and helps
         * reduce power consumption.
         *
         * Note:
         *  - Rarely used during normal operation
         *  - Mostly used in low-power or reset scenarios
         *************************************************************/

        if(pSPIx == SPI1)
        {
            /* Disable clock for SPI1 */
            SPI1_PCLK_DI();
        }
        else if(pSPIx == SPI2)
        {
            /* Disable clock for SPI2 */
            SPI2_PCLK_DI();
        }
        else if(pSPIx == SPI3)
        {
            /* Disable clock for SPI3 */
            SPI3_PCLK_DI();
        }
    }
}


/*********************************************************************
 * @fn          - SPI_Init
 *
 * @brief       - Initializes the SPI peripheral with given configuration
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle structure
 *
 * @return      - None
 *
 * @details
 *  This function configures the SPIx peripheral by programming
 *  the SPI_CR1 register based on user configuration.
 *
 *  IMPORTANT:
 *  - Only configuration is done here
 *  - SPI peripheral is NOT enabled (SPE bit = 0)
 *  - User must enable SPI after init using SPI_PeripheralControl()
 *
 *********************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
    uint32_t tempreg = 0;

    /*****************************************************************
     * STEP 1: Configure Device Mode (Master/Slave)
     *
     * Register : SPI_CR1
     * Bit      : MSTR (Bit 2)
     *
     * 0 → Slave mode
     * 1 → Master mode
     *
     * Why important?
     * - Master generates clock (SCLK)
     * - Slave depends on master clock
     *****************************************************************/
    tempreg |= (pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR);


    /*****************************************************************
     * STEP 2: Configure Bus Configuration
     *
     * SPI supports:
     * 1. Full Duplex (2-line unidirectional)
     * 2. Half Duplex (1-line bidirectional)
     * 3. Simplex RX-only
     *
     * Registers:
     *  - BIDIMODE (Bit 15)
     *  - RXONLY   (Bit 10)
     *****************************************************************/
    if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
    {
        /*
         * Full Duplex Mode
         * BIDIMODE = 0 → 2-line mode
         * RXONLY   = 0 → Enable both TX and RX
         */
        tempreg &= ~(1 << SPI_CR1_BIDIMODE);
        tempreg &= ~(1 << SPI_CR1_RXONLY);
    }
    else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
    {
        /*
         * Half Duplex Mode
         * BIDIMODE = 1 → 1-line bidirectional mode
         */
        tempreg |= (1 << SPI_CR1_BIDIMODE);
    }
    else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
    {
        /*
         * Simplex RX Only Mode
         * BIDIMODE = 0 → 2-line mode
         * RXONLY   = 1 → Disable TX, only RX
         */
        tempreg &= ~(1 << SPI_CR1_BIDIMODE);
        tempreg |= (1 << SPI_CR1_RXONLY);
    }


    /*****************************************************************
     * STEP 3: Configure SPI Serial Clock Speed
     *
     * Register : SPI_CR1
     * Bits     : BR[5:3]
     *
     * These bits divide APB clock to generate SPI clock.
     *
     * Example:
     *   000 → fPCLK/2
     *   001 → fPCLK/4
     *   ...
     *   111 → fPCLK/256
     *****************************************************************/
    tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);


    /*****************************************************************
     * STEP 4: Configure Data Frame Format (DFF)
     *
     * Register : SPI_CR1
     * Bit      : DFF (Bit 11)
     *
     * 0 → 8-bit data frame
     * 1 → 16-bit data frame
     *****************************************************************/
    tempreg |= (pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);


    /*****************************************************************
     * STEP 5: Configure Clock Polarity (CPOL)
     *
     * Register : SPI_CR1
     * Bit      : CPOL (Bit 1)
     *
     * 0 → Clock idle LOW
     * 1 → Clock idle HIGH
     *****************************************************************/
    tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);


    /*****************************************************************
     * STEP 6: Configure Clock Phase (CPHA)
     *
     * Register : SPI_CR1
     * Bit      : CPHA (Bit 0)
     *
     * 0 → Data captured on first edge
     * 1 → Data captured on second edge
     *
     * CPOL + CPHA together define SPI mode (Mode 0–3)
     *****************************************************************/
    tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);


    /*****************************************************************
     * STEP 7: Configure Software Slave Management (SSM)
     *
     * Register : SPI_CR1
     * Bits     : SSM (Bit 9), SSI (Bit 8)
     *
     * SSM:
     *  0 → Hardware slave management (NSS pin used)
     *  1 → Software slave management
     *
     * SSI:
     *  - Must be set when SSM = 1 (to avoid MODF error)
     *****************************************************************/
    tempreg |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

    if(pSPIHandle->SPIConfig.SPI_SSM == SPI_SSM_EN)
    {
        /*
         * Set SSI bit to HIGH
         *
         * Why?
         * - Prevents MODF (Mode Fault Error)
         * - Required when NSS is controlled by software
         */
        tempreg |= (1 << SPI_CR1_SSI);
    }


    /*****************************************************************
     * STEP 8: Write Configuration to SPI_CR1 Register
     *
     * All configurations are applied at once.
     *
     * Why use tempreg?
     * - Prevents partial writes
     * - Clean and safe configuration
     *****************************************************************/
    pSPIHandle->pSPIx->CR1 = tempreg;
}

/*********************************************************************
 * @fn          - SPI_DeInit
 *
 * @brief       - Reset the SPI peripheral to its default state
 *
 * @param[in]   - pSPIx : Base address of SPI peripheral (SPI1, SPI2, SPI3)
 *
 * @return      - None
 *
 * @details
 *  - This function de-initializes the SPI peripheral by forcing a
 *    hardware reset using RCC reset registers.
 *
 *  - Reset sequence:
 *        1. Set reset bit   → peripheral enters reset state
 *        2. Clear reset bit → peripheral comes out of reset
 *
 *  - After reset:
 *        → All SPI registers return to default reset values
 *        → Peripheral configuration is lost
 *
 *  - Useful when:
 *        → Re-initializing SPI with new configuration
 *        → Recovering from faulty configuration
 *
 *********************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
    /* Identify SPI peripheral using base address and reset it */

    if(pSPIx == SPI1)
    {
        /* SPI1 is on APB2 bus → use APB2 reset register */
        SPI1_REG_RESET();
    }
    else if(pSPIx == SPI2)
    {
        /* SPI2 is on APB1 bus → use APB1 reset register */
        SPI2_REG_RESET();
    }
    else if(pSPIx == SPI3)
    {
        /* SPI3 is on APB1 bus → use APB1 reset register */
        SPI3_REG_RESET();
    }
}

/*********************************************************************
 * @fn          - SPI_GetFlagStatus
 *
 * @brief       - Get the status (SET/RESET) of a specific SPI flag
 *
 * @param[in]   - pSPIx    : Base address of SPI peripheral
 * @param[in]   - FlagName : Flag to check (e.g., SPI_TXE_FLAG, SPI_RXNE_FLAG)
 *
 * @return      - FLAG_SET   → Flag is set (1)
 *                FLAG_RESET → Flag is cleared (0)
 *
 * @details
 *  - SPI peripheral updates status flags inside SR (Status Register)
 *  - Each flag represents a specific hardware condition
 *
 *  Example:
 *    TXE (Transmit buffer empty)
 *      → 1 = Ready to accept new data
 *      → 0 = Data register still busy
 *
 *  How it works:
 *    - SR & FlagName → isolates the required bit
 *    - If non-zero → flag is SET
 *
 *********************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
    if(pSPIx->SR & FlagName)   /* Check if specific flag bit is set */
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

/*********************************************************************
 * @fn          - SPI_SendData
 *
 * @brief       - Send data over SPI (Polling / Blocking mode)
 *
 * @param[in]   - pSPIx    : Base address of SPI peripheral
 * @param[in]   - pTxBuffer: Pointer to transmit buffer
 * @param[in]   - Len      : Number of bytes to send
 *
 * @return      - None
 *
 * @details
 *  - This is a blocking (polling-based) API
 *  - CPU waits until SPI is ready before sending next data
 *
 *  Transmission flow:
 *    1. Wait until TXE (Transmit buffer empty)
 *    2. Write data to DR (Data Register)
 *    3. Hardware shifts data out via MOSI
 *    4. Repeat until all data is sent
 *
 *  NOTE:
 *    - Supports both 8-bit and 16-bit data frame formats
 *
 *********************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx,uint8_t *pTxBuffer, uint32_t Len)
{
    while(Len > 0)
    {
        /* ---------------------------------------------------------
         * STEP 1: Wait until TXE (Transmit buffer empty)
         * ---------------------------------------------------------
         * TXE = 1 → DR is empty → safe to write new data
         * TXE = 0 → DR still holding previous data → wait
         */
        while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

        /* ---------------------------------------------------------
         * STEP 2: Check Data Frame Format (DFF)
         * ---------------------------------------------------------
         * DFF = 0 → 8-bit data
         * DFF = 1 → 16-bit data
         */
        if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
        {
            /* ================= 16-bit mode ================= */

            /* Write 16 bits into Data Register */
            pSPIx->DR = *((uint16_t*)pTxBuffer);

            /* Reduce length by 2 bytes */
            Len -= 2;

            /* Move buffer pointer by 2 bytes */
            pTxBuffer += 2;
        }
        else
        {
            /* ================= 8-bit mode ================= */

            /* Write 8 bits into Data Register */
            pSPIx->DR = *pTxBuffer;

            /* Reduce length by 1 byte */
            Len--;

            /* Move buffer pointer by 1 byte */
            pTxBuffer++;
        }
    }

    /* ---------------------------------------------------------
     * (IMPORTANT - OPTIONAL IMPROVEMENT)
     * Wait until SPI is not busy (BSY = 0)
     * Ensures last byte is fully transmitted on wire
     * ---------------------------------------------------------
     */
    while(SPI_GetFlagStatus(pSPIx, SPI_BSY_FLAG) == FLAG_SET);
}

/*********************************************************************
 * @fn          - SPI_ReceiveData
 *
 * @brief       - Receive data from SPI peripheral (Polling / Blocking mode)
 *
 * @param[in]   - pSPIx    : Base address of SPI peripheral
 * @param[in]   - pRxBuffer: Pointer to receive buffer
 * @param[in]   - Len      : Number of bytes to receive
 *
 * @return      - None
 *
 * @details
 *  - This is a blocking (polling-based) API
 *  - CPU waits until data is available before reading
 *
 *  IMPORTANT CONCEPT (VERY IMPORTANT):
 *  ----------------------------------
 *  SPI is FULL DUPLEX:
 *   → Data is received ONLY when clock is generated
 *
 *  So:
 *   - Master MUST send dummy data to generate clock
 *   - Slave responds during that clock
 *
 *  This API assumes:
 *   → Clock is already being generated (via TX or continuous transfer)
 *
 *  Reception flow:
 *   1. Wait until RXNE = 1 (data available in DR)
 *   2. Read data from DR
 *   3. Store into buffer
 *   4. Repeat until all data received
 *
 *********************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
    while(Len > 0)
    {
        /* ---------------------------------------------------------
         * STEP 1: Wait until RXNE (Receive buffer not empty)
         * ---------------------------------------------------------
         * RXNE = 1 → Data is available in DR → safe to read
         * RXNE = 0 → No data received yet → wait
         */
        while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

        /* ---------------------------------------------------------
         * STEP 2: Check Data Frame Format (DFF)
         * ---------------------------------------------------------
         * DFF = 0 → 8-bit data
         * DFF = 1 → 16-bit data
         */
        if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
        {
            /* ================= 16-bit mode ================= */

            /*
             * Read 16-bit data from Data Register (DR)
             * Casting ensures correct data width access
             */
            *((uint16_t*)pRxBuffer) = (uint16_t)pSPIx->DR;

            /* Reduce length by 2 bytes */
            Len -= 2;

            /* Move buffer pointer by 2 bytes */
            pRxBuffer += 2;
        }
        else
        {
            /* ================= 8-bit mode ================= */

            /*
             * Read 8-bit data from Data Register (DR)
             */
            *pRxBuffer = (uint8_t)pSPIx->DR;

            /* Reduce length by 1 byte */
            Len--;

            /* Move buffer pointer by 1 byte */
            pRxBuffer++;
        }
    }
}

/*********************************************************************
 * @fn          - SPI_IRQInterruptConfig
 *
 * @brief       - Enable or Disable NVIC interrupt for given IRQ number
 *
 * @param[in]   - IRQNumber : Interrupt number (from vector table)
 * @param[in]   - EnorDi    : ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - NVIC (Nested Vector Interrupt Controller) controls interrupts
 *  - Interrupts are enabled/disabled using:
 *
 *      ISERx → Interrupt Set Enable Register
 *      ICERx → Interrupt Clear Enable Register
 *
 *  - Each register controls 32 interrupts:
 *
 *      ISER0 → IRQ 0–31
 *      ISER1 → IRQ 32–63
 *      ISER2 → IRQ 64–95
 *
 *  - Bit position inside register:
 *        bit = IRQNumber % 32
 *
 *********************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        /* ================= ENABLE INTERRUPT ================= */

        if(IRQNumber <= 31)
        {
            /* ISER0 handles IRQ 0–31 */
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber <= 63)
        {
            /* ISER1 handles IRQ 32–63 */
            *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber <= 95)
        {
            /* ISER2 handles IRQ 64–95 */
            *NVIC_ISER2 |= (1 << (IRQNumber % 32));
        }
    }
    else
    {
        /* ================= DISABLE INTERRUPT ================= */

        if(IRQNumber <= 31)
        {
            /* ICER0 handles IRQ 0–31 */
            *NVIC_ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber <= 63)
        {
            /* ICER1 handles IRQ 32–63 */
            *NVIC_ICER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber <= 95)
        {
            /* ICER2 handles IRQ 64–95 */
            *NVIC_ICER2 |= (1 << (IRQNumber % 32));
        }
    }
}

/*********************************************************************
 * @fn          - SPI_IRQPriorityConfig
 *
 * @brief       - Configure priority of a given IRQ in NVIC
 *
 * @param[in]   - IRQNumber   : Interrupt number
 * @param[in]   - IRQPriority : Priority level (0–15 for STM32F4)
 *
 * @return      - None
 *
 * @details
 *  - NVIC supports programmable interrupt priority
 *  - Priority registers: NVIC_IPR[x]
 *
 *  - Each IPR register holds 4 IRQs
 *      → Each IRQ gets 8 bits
 *
 *  - STM32F407 uses only upper 4 bits [7:4]
 *
 *  Example layout:
 *  -------------------------------------------------------
 *  | IRQ3 | IRQ2 | IRQ1 | IRQ0 |
 *  |31-24 |23-16 |15-8  |7-0   |
 *  -------------------------------------------------------
 *
 *  - Only upper nibble is valid:
 *      priority << 4
 *
 *********************************************************************/
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    /*****************************************************************
     * STEP 1: Find IPR register index
     *
     * Each register holds 4 IRQs
     * Example:
     *   IPR0 → IRQ0–3
     *   IPR1 → IRQ4–7
     *****************************************************************/
    uint8_t iprx = IRQNumber / 4;

    /*****************************************************************
     * STEP 2: Find section within IPR register
     *
     * section = position of IRQ inside register (0 to 3)
     *****************************************************************/
    uint8_t section = IRQNumber % 4;

    /*****************************************************************
     * STEP 3: Calculate bit shift
     *
     * Each IRQ occupies 8 bits
     * But only upper 4 bits are implemented
     *
     * shift = (8 * section) + 4
     *****************************************************************/
    uint8_t shift = (8 * section) + 4;

    /*****************************************************************
     * STEP 4: Clear existing priority (important)
     *****************************************************************/
    *(NVIC_PR_BASEADDR + iprx) &= ~(0xFF << (8 * section));

    /*****************************************************************
     * STEP 5: Set new priority
     *
     * Only upper 4 bits are used by hardware
     *****************************************************************/
    *(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift);
}

/*********************************************************************
 * @fn          - SPI_SendDataIT
 *
 * @brief       - Initiate SPI data transmission in interrupt mode
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle structure
 * @param[in]   - pTxBuffer  : Pointer to transmit buffer
 * @param[in]   - Len        : Length of data to transmit
 *
 * @return      - Current SPI transmission state
 *
 * @details
 *  - This is a NON-BLOCKING API
 *  - Actual transmission happens inside ISR (interrupt handler)
 *
 *  FLOW:
 *  ------------------------------------------------------------
 *   1. Check if SPI is busy
 *   2. Store buffer & length in handle
 *   3. Mark SPI as BUSY
 *   4. Enable TXE interrupt
 *   5. ISR takes over transmission
 *
 *********************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer, uint32_t Len)
{
    /* Read current transmission state */
    uint8_t state = pSPIHandle->TxState;

    /* Proceed only if SPI is not busy */
    if(state != SPI_BUSY_IN_TX)
    {
        /* Save buffer address */
        pSPIHandle->pTxBuffer = pTxBuffer;

        /* Save total length */
        pSPIHandle->TxLen = Len;

        /* Mark SPI as busy in transmission */
        pSPIHandle->TxState = SPI_BUSY_IN_TX;

        /* Enable TXE interrupt → triggers ISR */
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
    }

    /* Return previous state */
    return state;
}


/*********************************************************************
 * @fn          - SPI_ReceiveDataIT
 *
 * @brief       - Initiate SPI data reception in interrupt mode
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle
 * @param[in]   - pRxBuffer  : Pointer to receive buffer
 * @param[in]   - Len        : Number of bytes to receive
 *
 * @return      - Current SPI reception state
 *
 * @details
 *  - Non-blocking API
 *  - Data reception handled inside ISR
 *
 *  NOTE:
 *  ------------------------------------------------------------
 *  SPI is FULL DUPLEX → clock must be generated externally
 *
 *********************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
    uint8_t state = pSPIHandle->RxState;

    if(state != SPI_BUSY_IN_RX)
    {
        /* Store receive buffer */
        pSPIHandle->pRxBuffer = pRxBuffer;

        /* Store length */
        pSPIHandle->RxLen = Len;

        /* Mark SPI busy */
        pSPIHandle->RxState = SPI_BUSY_IN_RX;

        /* Enable RXNE interrupt */
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
    }

    return state;
}


/*********************************************************************
 * @fn          - spi_txe_interrupt_handle
 *
 * @brief       - Handle TXE interrupt (Transmit buffer empty)
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle
 *
 * @details
 *  - Called when TXE flag is set
 *  - Loads data into DR register
 *  - Handles both 8-bit and 16-bit modes
 *
 *********************************************************************/
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    /* Check DFF bit → data frame format */
    if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
    {
        /* ----------- 16-bit mode ----------- */

        /* Load 16-bit data into DR */
        pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);

        /* Reduce length by 2 bytes */
        pSPIHandle->TxLen -= 2;

        /* Move buffer pointer by 2 bytes */
        pSPIHandle->pTxBuffer += 2;
    }
    else
    {
        /* ----------- 8-bit mode ----------- */

        /* Load 8-bit data */
        pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;

        /* Reduce length */
        pSPIHandle->TxLen--;

        /* Move buffer pointer */
        pSPIHandle->pTxBuffer++;
    }

    /* If all data transmitted */
    if(!pSPIHandle->TxLen)
    {
        /* Close transmission */
        SPI_CloseTransmisson(pSPIHandle);

        /* Notify application */
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
    }
}


/*********************************************************************
 * @fn          - spi_rxne_interrupt_handle
 *
 * @brief       - Handle RXNE interrupt (Receive buffer not empty)
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle
 *
 * @details
 *  - Called when data is received
 *  - Reads data from DR register
 *
 *********************************************************************/
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    /* Check DFF */
    if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
    {
        /* ----------- 16-bit mode ----------- */

        *((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;

        pSPIHandle->RxLen -= 2;

        pSPIHandle->pRxBuffer += 2;
    }
    else
    {
        /* ----------- 8-bit mode ----------- */

        *(pSPIHandle->pRxBuffer) = (uint8_t)pSPIHandle->pSPIx->DR;

        pSPIHandle->RxLen--;

        pSPIHandle->pRxBuffer++;
    }

    /* If reception complete */
    if(!pSPIHandle->RxLen)
    {
        SPI_CloseReception(pSPIHandle);

        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
    }
}


/*********************************************************************
 * @fn          - spi_ovr_err_interrupt_handle
 *
 * @brief       - Handle Overrun (OVR) error interrupt
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle
 *
 * @details
 *  - OVR occurs when data is not read in time
 *  - Must be cleared manually
 *
 *********************************************************************/
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    uint8_t temp;

    /* Clear OVR only if not transmitting */
    if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
    {
        temp = pSPIHandle->pSPIx->DR;
        temp = pSPIHandle->pSPIx->SR;
    }

    (void)temp;

    /* Notify application */
    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}


/*********************************************************************
 * @fn          - SPI_IRQHandling
 *
 * @brief       - Main SPI interrupt handler
 *
 * @param[in]   - pHandle : Pointer to SPI handle
 *
 * @details
 *  - This function is called from ISR (vector table)
 *  - It checks which interrupt occurred and calls respective handler
 *
 *********************************************************************/
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
    uint8_t temp1, temp2;

    /* ----------- TXE Interrupt ----------- */
    temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

    if(temp1 && temp2)
    {
        spi_txe_interrupt_handle(pHandle);
    }

    /* ----------- RXNE Interrupt ----------- */
    temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

    if(temp1 && temp2)
    {
        spi_rxne_interrupt_handle(pHandle);
    }

    /* ----------- OVR Error ----------- */
    temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

    if(temp1 && temp2)
    {
        spi_ovr_err_interrupt_handle(pHandle);
    }
}

/*********************************************************************
 * @fn          - SPI_PeripheralControl
 *
 * @brief       - Enable or disable the SPI peripheral
 *
 * @param[in]   - pSPIx  : Base address of SPI peripheral (SPI1/SPI2/SPI3)
 * @param[in]   - EnorDi : ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - Controls the SPE (SPI Enable) bit in SPI_CR1 register.
 *
 *  SPE (Bit 6):
 *    0 → SPI disabled
 *    1 → SPI enabled
 *
 *  WHY this API is required?
 *  ------------------------------------------------------------
 *  - SPI configuration (CR1 settings) can be done while disabled
 *  - But actual communication starts ONLY when SPE = 1
 *
 *  IMPORTANT NOTES:
 *  - Call this AFTER SPI_Init()
 *  - Before disabling SPI:
 *        → Ensure BSY flag is RESET (no ongoing transmission)
 *        → Otherwise last data may get corrupted
 *
 *********************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        /* Enable SPI peripheral (start communication capability) */
        pSPIx->CR1 |= (1 << SPI_CR1_SPE);
    }
    else
    {
        /* Disable SPI peripheral */
        pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
    }
}

/*********************************************************************
 * @fn          - SPI_SSIConfig
 *
 * @brief       - Configure Internal Slave Select (SSI bit)
 *
 * @param[in]   - pSPIx  : Base address of SPI peripheral
 * @param[in]   - EnorDi : ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - Controls SSI (Internal Slave Select) bit in SPI_CR1.
 *
 *  SSI (Bit 8):
 *    0 → Internal NSS = LOW
 *    1 → Internal NSS = HIGH
 *
 *  USED ONLY WHEN:
 *    → SSM (Software Slave Management) = 1
 *
 *  WHY SSI is important?
 *  ------------------------------------------------------------
 *  - When SSM = 1, hardware NSS pin is ignored
 *  - But internally SPI still needs NSS = HIGH in master mode
 *
 *  If SSI is NOT set:
 *    → MODF (Mode Fault) error occurs
 *    → SPI gets disabled automatically
 *
 *  So:
 *    SSM = 1  → MUST set SSI = 1
 *
 *********************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        /* Set SSI → simulate NSS = HIGH internally */
        pSPIx->CR1 |= (1 << SPI_CR1_SSI);
    }
    else
    {
        /* Clear SSI */
        pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
    }
}

/*********************************************************************
 * @fn          - SPI_SSOEConfig
 *
 * @brief       - Configure Slave Select Output Enable (SSOE)
 *
 * @param[in]   - pSPIx  : Base address of SPI peripheral
 * @param[in]   - EnOrDi : ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - Controls SSOE bit in SPI_CR2 register.
 *
 *  SSOE (Bit 2):
 *    0 → NSS pin is free (software/general GPIO control)
 *    1 → NSS managed automatically by hardware
 *
 *  USED IN:
 *    → Master mode with hardware NSS management
 *
 *  HOW IT WORKS:
 *  ------------------------------------------------------------
 *  When SSOE = 1 and SPE = 1:
 *    → NSS pin is automatically pulled LOW (select slave)
 *
 *  When SPE = 0:
 *    → NSS pin goes HIGH (deselect slave)
 *
 *  WHY this is useful?
 *  ------------------------------------------------------------
 *  - Eliminates manual GPIO control of NSS
 *  - Ensures proper timing of slave selection
 *
 *  NOTE:
 *  - Not used when SSM = 1 (software NSS mode)
 *  - Used when SSM = 0 (hardware NSS mode)
 *
 *********************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        /* Enable hardware NSS output */
        pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
    }
    else
    {
        /* Disable hardware NSS control */
        pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
    }
}


/*********************************************************************
 * @fn          - SPI_CloseTransmisson
 *
 * @brief       - Close/terminate SPI transmission (Interrupt mode)
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle structure
 *
 * @return      - None
 *
 * @details
 *  - This function is called when transmission is complete
 *  - It disables TX interrupt and resets all transmission-related data
 *
 *  WHY THIS FUNCTION IS REQUIRED?
 *  ------------------------------------------------------------
 *  - Prevents continuous TXE interrupts after transmission ends
 *  - Clears buffer and length to avoid stale data usage
 *  - Marks SPI as READY for next transmission
 *
 *********************************************************************/
void SPI_CloseTransmisson(SPI_Handle_t *pSPIHandle)
{
    /* Disable TXE interrupt (TX buffer empty interrupt)
     * This stops further interrupt triggering once transmission is done
     */
    pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);

    /* Clear transmit buffer pointer
     * Avoids accidental reuse of old buffer data
     */
    pSPIHandle->pTxBuffer = NULL;

    /* Reset transmission length
     * Indicates no more data left to transmit
     */
    pSPIHandle->TxLen = 0;

    /* Update SPI state to READY
     * Allows new transmission to start
     */
    pSPIHandle->TxState = SPI_READY;
}


/*********************************************************************
 * @fn          - SPI_CloseReception
 *
 * @brief       - Close/terminate SPI reception (Interrupt mode)
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle structure
 *
 * @return      - None
 *
 * @details
 *  - Called when reception is complete
 *  - Disables RX interrupt and resets reception-related data
 *
 *  PURPOSE:
 *  ------------------------------------------------------------
 *  - Prevents continuous RXNE interrupts
 *  - Clears buffer and resets length
 *  - Prepares SPI for next reception
 *
 *********************************************************************/
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
    /* Disable RXNE interrupt (Receive buffer not empty interrupt)
     * Prevents ISR from firing after reception completes
     */
    pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);

    /* Clear receive buffer pointer
     * Avoids accidental use of old data buffer
     */
    pSPIHandle->pRxBuffer = NULL;

    /* Reset reception length
     * No more data to receive
     */
    pSPIHandle->RxLen = 0;

    /* Update SPI state to READY
     * SPI is now free for next operation
     */
    pSPIHandle->RxState = SPI_READY;
}


/*********************************************************************
 * @fn          - SPI_ClearOVRFlag
 *
 * @brief       - Clear Overrun (OVR) error flag in SPI
 *
 * @param[in]   - pSPIx : Base address of SPI peripheral
 *
 * @return      - None
 *
 * @details
 *  - OVR (Overrun Error) occurs when:
 *      → New data arrives before previous data is read
 *
 *  - If OVR is not cleared:
 *      → SPI communication stops working properly
 *
 *  HOW TO CLEAR OVR FLAG (as per reference manual):
 *  ------------------------------------------------------------
 *   1. Read Data Register (DR)
 *   2. Then read Status Register (SR)
 *
 *********************************************************************/
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
    uint8_t temp;

    /* Step 1: Read data register (DR)
     * This clears part of the OVR condition
     */
    temp = pSPIx->DR;

    /* Step 2: Read status register (SR)
     * Completes clearing of OVR flag
     */
    temp = pSPIx->SR;

    /* Avoid compiler warning for unused variable */
    (void)temp;
}


/*********************************************************************
 * @fn          - SPI_ApplicationEventCallback
 *
 * @brief       - Callback function for SPI events (User override)
 *
 * @param[in]   - pSPIHandle : Pointer to SPI handle structure
 * @param[in]   - AppEv      : Event type
 *
 * @return      - None
 *
 * @details
 *  - This is a WEAK function → user can override it in application
 *
 *  EVENTS:
 *  ------------------------------------------------------------
 *   SPI_EVENT_TX_CMPLT → Transmission complete
 *   SPI_EVENT_RX_CMPLT → Reception complete
 *   SPI_EVENT_OVR_ERR  → Overrun error
 *   SPI_EVENT_CRC_ERR  → CRC error
 *
 *  PURPOSE:
 *  ------------------------------------------------------------
 *  - Notifies application when interrupt-driven operation finishes
 *
 *  HOW USER USES THIS:
 *  ------------------------------------------------------------
 *  - User defines same function in application code
 *  - This weak version gets overridden automatically
 *
 *********************************************************************/
__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
    /* Weak implementation (default)
     * User application can override this function
     * to handle SPI events like TX complete, RX complete, errors, etc.
     */
}

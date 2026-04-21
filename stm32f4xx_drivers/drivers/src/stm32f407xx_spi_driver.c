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

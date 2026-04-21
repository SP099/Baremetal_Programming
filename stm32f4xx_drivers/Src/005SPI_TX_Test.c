/*
 * 005SPI_TX_Test.c
 *
 *  Created on: Apr 20, 2026
 *      Author: Lenovo
 */

#include <string.h>
#include "stm32f407xx.h"

/*********************************************************************
 * @fn          - SPI2_GPIOInits
 *
 * @brief       - Initialize GPIO pins for SPI2 functionality
 *
 * @details
 *  - Configures PB13 (SCLK) and PB15 (MOSI) in Alternate Function mode
 *  - AF5 is used for SPI2
 *  - MISO and NSS are not required for TX-only test
 *********************************************************************/
void SPI2_GPIOInits(void)
{
    GPIO_Handle_t SPIPins;

    /* STEP 1: Enable clock for GPIOB peripheral */
    GPIO_PeriClockControl(GPIOB, ENABLE);

    /* STEP 2: Configure common GPIO settings */
    SPIPins.pGPIOx = GPIOB;
    SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;   // Alternate function mode
    SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;           // AF5 → SPI2
    SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP; // Push-pull output
    SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; // No pull-up/pull-down
    SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;  // Fast speed

    /* STEP 3: Configure SCLK (PB13) */
    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    GPIO_Init(&SPIPins);

    /* STEP 4: Configure MOSI (PB15) */
    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
    GPIO_Init(&SPIPins);

    /*
     * NOTE:
     * - MISO is not configured because we are not receiving data
     * - NSS is not used because SSM (software slave management) is enabled
     */
}

/*********************************************************************
 * @fn          - SPI2_Inits
 *
 * @brief       - Initialize SPI2 peripheral with required configuration
 *
 * @details
 *  - Configures SPI in Master mode
 *  - Full duplex (TX + RX, but we use only TX)
 *  - 8-bit data frame
 *  - Mode 0 (CPOL=0, CPHA=0)
 *  - Software slave management enabled
 *********************************************************************/
void SPI2_Inits(void)
{
    SPI_Handle_t SPI2handle;

    /* STEP 1: Select SPI2 peripheral */
    SPI2handle.pSPIx = SPI2;

    /* STEP 2: Enable SPI2 peripheral clock */
    SPI_PeriClockControl(SPI2, ENABLE);

    /* STEP 3: Configure SPI parameters */
    SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;      // Full duplex
    SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;// Master mode
    SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;    // Fastest (depends on PCLK)
    SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;                // 8-bit data

    /* SPI Mode 0 → CPOL=0, CPHA=0 */
    SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
    SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;

    /* Enable software slave management */
    SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

    /* STEP 4: Initialize SPI peripheral */
    SPI_Init(&SPI2handle);
}

/*********************************************************************
 * @fn          - main
 *
 * @brief       - Entry point of program
 *
 * @details
 *  Flow:
 *   1. Initialize GPIO pins for SPI
 *   2. Initialize SPI peripheral
 *   3. Set SSI (internal NSS high)
 *   4. Enable SPI
 *   5. Send data
 *   6. Wait for transmission completion
 *   7. Disable SPI
 *********************************************************************/
int main(void)
{
    char user_data[] = "Hello world";

    /* STEP 1: Initialize GPIO pins */
    SPI2_GPIOInits();

    /* STEP 2: Initialize SPI peripheral */
    SPI2_Inits();

    /* STEP 3: Set SSI = 1
     *
     * WHY?
     * - Required when SSM = 1
     * - Prevents MODF (Mode Fault error)
     */
    SPI_SSIConfig(SPI2, ENABLE);

    /* STEP 4: Enable SPI peripheral (SPE = 1) */
    SPI_PeriPheralControl(SPI2, ENABLE);

    /* STEP 5: Send data (blocking call)
     *
     * Internally:
     * - Waits for TXE = 1
     * - Writes data to DR
     * - Hardware shifts data out on MOSI
     */
    SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

    /* STEP 6: Ensure transmission is fully completed */

    /* Wait until TX buffer is empty */
    while(SPI_GetFlagStatus(SPI2, SPI_TXE_FLAG) == FLAG_RESET);

    /* Wait until SPI is not busy (last bit transmitted) */
    while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG) == FLAG_SET);

    /* STEP 7: Disable SPI peripheral */
    SPI_PeriPheralControl(SPI2, DISABLE);

    /* Infinite loop */
    while(1);

    return 0;
}

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

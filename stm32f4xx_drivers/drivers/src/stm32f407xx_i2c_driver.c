/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Apr 22, 2026
 *      Author: Lenovo
 */

#include "stm32f407xx_i2c_driver.h"

/*********************************************************************
 * @fn          - I2C_PeriClockControl
 *
 * @brief       - Enable or disable clock for I2C peripheral
 *
 * @param[in]   - pI2Cx : Pointer to I2C peripheral (I2C1/I2C2/I2C3)
 * @param[in]   - EnorDi: ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - Every peripheral needs clock before use
 *  - Clock is controlled via RCC registers
 *
 *  FLOW:
 *   ENABLE:
 *     → Sets corresponding bit in RCC APB1ENR register
 *     → Peripheral becomes active
 *
 *   DISABLE:
 *     → Clears corresponding bit
 *     → Saves power
 *
 *********************************************************************/
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        // Enable clock for selected I2C peripheral
        if(pI2Cx == I2C1)      I2C1_PCLK_EN();
        else if(pI2Cx == I2C2) I2C2_PCLK_EN();
        else if(pI2Cx == I2C3) I2C3_PCLK_EN();
    }
    else
    {
        // Disable clock for selected I2C peripheral
        if(pI2Cx == I2C1)      I2C1_PCLK_DI();
        else if(pI2Cx == I2C2) I2C2_PCLK_DI();
        else if(pI2Cx == I2C3) I2C3_PCLK_DI();
    }
}

/*********************************************************************
 * @fn          - I2C_DeInit
 *
 * @brief       - Reset I2C peripheral registers to default values
 *
 * @param[in]   - pI2Cx : Pointer to I2C peripheral
 *
 * @return      - None
 *
 * @details
 *  - This resets the peripheral using RCC reset register
 *  - All registers return to power-on reset state
 *
 *  WHY NEEDED?
 *   → Clean reinitialization
 *   → Debugging faulty configurations
 *
 *********************************************************************/
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
    if(pI2Cx == I2C1)      I2C1_REG_RESET();
    else if(pI2Cx == I2C2) I2C2_REG_RESET();
    else if(pI2Cx == I2C3) I2C3_REG_RESET();
}

/*********************************************************************
 * @fn          - I2C_IRQInterruptConfig
 *
 * @brief       - Enable or disable NVIC interrupt for I2C
 *
 * @param[in]   - IRQNumber : IRQ number of I2C peripheral
 * @param[in]   - EnorDi    : ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - NVIC handles all interrupts in Cortex-M
 *
 *  REGISTERS:
 *   ISERx → Enable interrupt
 *   ICERx → Disable interrupt
 *
 *  LOGIC:
 *   - Each register handles 32 interrupts
 *   - IRQNumber determines register and bit position
 *
 *********************************************************************/
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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
 * @fn          - I2C_IRQPriorityConfig
 *
 * @brief       - Configure priority of I2C interrupt
 *
 * @param[in]   - IRQNumber   : IRQ number
 * @param[in]   - IRQPriority : Priority value (0–15)
 *
 * @return      - None
 *
 * @details
 *  - NVIC allows priority-based interrupt handling
 *
 *  IMPORTANT:
 *   - Each IPR register handles 4 interrupts
 *   - Each interrupt uses 8 bits (only upper 4 used)
 *
 *  STEPS:
 *   1. Find register index (IRQNumber / 4)
 *   2. Find section (IRQNumber % 4)
 *   3. Shift priority into correct position
 *
 *********************************************************************/
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
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
 * @fn          - I2C_PeripheralControl
 *
 * @brief       - Enable or disable I2C peripheral
 *
 * @param[in]   - pI2Cx : Pointer to I2C peripheral
 * @param[in]   - EnorDi: ENABLE or DISABLE
 *
 * @return      - None
 *
 * @details
 *  - Controls PE (Peripheral Enable) bit in CR1 register
 *
 *  PE BIT:
 *   0 → Peripheral disabled
 *   1 → Peripheral enabled
 *
 *  NOTE:
 *   - Must be enabled before communication starts
 *   - Disable before reconfiguration
 *
 *********************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        // Enable I2C peripheral
        pI2Cx->CR1 |= (1 << I2C_CR1_PE);
    }
    else
    {
        // Disable I2C peripheral
        pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
    }
}

/*********************************************************************
 * @fn          - I2C_GetFlagStatus
 *
 * @brief       - Check status of a specific I2C flag
 *
 * @param[in]   - pI2Cx   : Pointer to I2C peripheral
 * @param[in]   - FlagName: Flag to check (SR1/SR2 flags)
 *
 * @return      - FLAG_SET or FLAG_RESET
 *
 * @details
 *  - Used to monitor events like:
 *      SB   → Start condition generated
 *      ADDR → Address matched
 *      TXE  → Data register empty
 *      RXNE → Data received
 *
 *********************************************************************/
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Apr 18, 2026
 *      Author: Lenovo
 */
#include "stm32f407xx_gpio_driver.h"

/*********************************************************************
 * @fn      		  - GPIO_PeriClockControl
 *
 * @brief             - Enable or disable the peripheral clock for a given GPIO port
 *
 * @param[in]         - pGPIOx: Base address of the GPIO peripheral (GPIOA, GPIOB, etc.)
 * @param[in]         - EnorDi: ENABLE or DISABLE macro
 *                        ENABLE  -> Enables the clock for the GPIO port
 *                        DISABLE -> Disables the clock for the GPIO port
 *
 * @return            - None
 *
 * @Note              - Peripheral clock must be enabled before configuring or using GPIO registers
 *                      Disabling the clock is optional and typically used for power saving
 *
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	/*
	 * Enable clock for the selected GPIO peripheral
	 */
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();   /* Enable clock for GPIOA (AHB1ENR bit 0) */
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();   /* Enable clock for GPIOB (AHB1ENR bit 1) */
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();   /* Enable clock for GPIOC (AHB1ENR bit 2) */
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();   /* Enable clock for GPIOD (AHB1ENR bit 3) */
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();   /* Enable clock for GPIOE (AHB1ENR bit 4) */
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();   /* Enable clock for GPIOF (AHB1ENR bit 5) */
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_EN();   /* Enable clock for GPIOG (AHB1ENR bit 6) */
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();   /* Enable clock for GPIOH (AHB1ENR bit 7) */
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_EN();   /* Enable clock for GPIOI (AHB1ENR bit 8) */
		}
	}
	else
	{
		/*
		 * Disable clock for the selected GPIO peripheral
		 * (Used mainly for power saving)
		 */
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();   /* Disable clock for GPIOA */
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();   /* Disable clock for GPIOB */
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();   /* Disable clock for GPIOC */
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();   /* Disable clock for GPIOD */
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();   /* Disable clock for GPIOE */
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_DI();   /* Disable clock for GPIOF */
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_DI();   /* Disable clock for GPIOG */
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();   /* Disable clock for GPIOH */
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_DI();   /* Disable clock for GPIOI */
		}
	}
}


/*********************************************************************
 * @fn                   - GPIO_Init
 *
 * @brief                - Initializes a GPIO pin based on the configuration
 *                         provided in the GPIO_Handle_t structure
 *
 * @param[in]            - pGPIOHandle: Pointer to GPIO handle structure
 *                         which contains:
 *                         - GPIO port base address
 *                         - GPIO pin configuration settings
 *
 * @return               - None
 *
 * @Note                 - This function configures the following:
 *                         - Pin mode (Input/Output/Alternate/Analog/Interrupt)
 *                         - Output type (Push-pull/Open-drain)
 *                         - Output speed
 *                         - Pull-up/Pull-down settings
 *                         - Alternate function (if applicable)
 *
 *                         Peripheral clock must be enabled before calling this API
 *
 *********************************************************************/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
    uint8_t pin = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;

    /*********************************************************************
     * 1. CONFIGURE GPIO MODE (MODER register)
     *
     * Each GPIO pin uses 2 bits:
     *   Pin 0 → bits [1:0]
     *   Pin 1 → bits [3:2]
     *   Pin n → bits [(2n+1):(2n)]
     *
     * Why (2 * pin)?
     *   - Because each pin occupies 2 bits
     *   - So starting bit position = 2 × pin
     *
     * Why 0x03?
     *   - 0x03 = binary 0000 0011 → exactly 2 bits
     *   - We need a mask that matches field width (2 bits)
     *
     * Clearing logic:
     *   ~(0x03 << (2 * pin)) → creates mask like 1111 0011
     *   AND operation clears only those 2 bits
     *
     * Example:
     *   pin = 1 → position = 2
     *
     *   Step 1: Create mask
     *     0x03 << 2 = 0000 1100   (selects bits [3:2])
     *
     *   Step 2: Invert mask
     *     ~(0000 1100) = 1111 0011   (only target bits become 0)
     *
     *   Step 3: Apply AND
     *     REG = REG & 1111 0011
     *
     * Why AND clears bits?
     *   - 1 & X = X  → keeps bit unchanged
     *   - 0 & X = 0  → forces bit to 0
     *
     * Why clear before setting?
     *   - OR operation can only set bits (1), not reset them
     *   - So we must clear old value before writing new one
     *
     *********************************************************************/
    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
    {
        /* Clear existing 2 bits */
        pGPIOHandle->pGPIOx->MODER &= ~(0x03 << (2 * pin));

        /* Set new mode */
        pGPIOHandle->pGPIOx->MODER |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pin));
    }
    else
    {
        // TODO: Interrupt mode configuration (EXTI, SYSCFG, NVIC)
    }

    /*********************************************************************
     * 2. CONFIGURE OUTPUT SPEED (OSPEEDR register)
     *
     * Same layout as MODER → 2 bits per pin
     *
     * Why same logic?
     *   - Hardware uses identical bit-field design
     *
     * Why mask with 0x03?
     *   - Ensures only valid 2-bit value is written (0–3)
     *   - Prevents corruption if user gives invalid input
     *
     *********************************************************************/
    pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x03 << (2 * pin));
    pGPIOHandle->pGPIOx->OSPEEDR |= ((pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed & 0x03) << (2 * pin));

    /*********************************************************************
     * 3. CONFIGURE PULL-UP / PULL-DOWN (PUPDR register)
     *
     * Again 2 bits per pin:
     *   00 → No pull
     *   01 → Pull-up
     *   10 → Pull-down
     *
     * Why 0x03?
     *   - Field width = 2 bits → mask must be 2 bits
     *
     * Why mask input?
     *   - Safety: ensures only valid encoding is written
     *
     *********************************************************************/
    pGPIOHandle->pGPIOx->PUPDR &= ~(0x03 << (2 * pin));
    pGPIOHandle->pGPIOx->PUPDR |= ((pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl & 0x03) << (2 * pin));

    /*********************************************************************
     * 4. CONFIGURE OUTPUT TYPE (OTYPER register)
     *
     * Each pin uses ONLY 1 bit:
     *   Bit n → controls pin n
     *
     * Why (1 << pin)?
     *   - Because each pin occupies exactly 1 bit
     *
     * Why NOT (2 * pin)?
     *   - That applies only to 2-bit fields (MODER, OSPEEDR, PUPDR)
     *
     * Why 0x01 mask?
     *   - Only 1-bit valid value (0 or 1)
     *
     *********************************************************************/
    pGPIOHandle->pGPIOx->OTYPER &= ~(1 << pin);
    pGPIOHandle->pGPIOx->OTYPER |= ((pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType & 0x01) << pin);

    /*********************************************************************
     * 5. CONFIGURE ALTERNATE FUNCTION (AFR[0] / AFR[1])
     *
     * Each pin uses 4 bits:
     *   AFR[0] → pins 0–7
     *   AFR[1] → pins 8–15
     *
     * Why (pin / 8)?
     *   - Each AFR register handles 8 pins
     *   - So we select register index (0 or 1)
     *
     * Why (pin % 8) * 4?
     *   - Each pin occupies 4 bits
     *   - Multiply by 4 to get correct bit position
     *
     * Why 0x0F?
     *   - 0x0F = binary 0000 1111 → 4-bit mask
     *
     * Why mask input with 0x0F?
     *   - Ensures only valid AF value (0–15) is written
     *
     *********************************************************************/
    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
    {
        uint8_t reg_index = pin / 8;
        uint8_t bit_pos   = (pin % 8) * 4;

        /* Clear existing 4 bits */
        pGPIOHandle->pGPIOx->AFR[reg_index] &= ~(0x0F << bit_pos);

        /* Set alternate function */
        pGPIOHandle->pGPIOx->AFR[reg_index] |=
            ((pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode & 0x0F) << bit_pos);
    }
}

/*********************************************************************
 * @fn                  - GPIO_DeInit
 *
 * @brief               - Reset the given GPIO peripheral to its default state
 *
 * @param[in]           - pGPIOx: Base address of the GPIO port (GPIOA, GPIOB, etc.)
 *
 * @return              - None
 *
 * @Note                - This function resets the GPIO port using RCC reset registers,
 *                        restoring all its registers to reset values.
 *                      - De-initialize GPIO by toggling its reset bit in RCC AHB1RSTR register (set → clear)
 *
 *********************************************************************/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
    if(pGPIOx == GPIOA)
    {
        GPIOA_REG_RESET();
    }
    else if(pGPIOx == GPIOB)
    {
        GPIOB_REG_RESET();
    }
    else if(pGPIOx == GPIOC)
    {
        GPIOC_REG_RESET();
    }
    else if(pGPIOx == GPIOD)
    {
        GPIOD_REG_RESET();
    }
    else if(pGPIOx == GPIOE)
    {
        GPIOE_REG_RESET();
    }
    else if(pGPIOx == GPIOF)
    {
        GPIOF_REG_RESET();
    }
    else if(pGPIOx == GPIOG)
    {
        GPIOG_REG_RESET();
    }
    else if(pGPIOx == GPIOH)
    {
        GPIOH_REG_RESET();
    }
    else if(pGPIOx == GPIOI)
    {
        GPIOI_REG_RESET();
    }
}


/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPin
 *
 * @brief             - Read the logic level (HIGH/LOW) from a specific GPIO input pin
 *
 * @param[in]         - pGPIOx: Base address of the GPIO peripheral
 * @param[in]         - PinNumber: GPIO pin number (0 to 15)
 *
 * @return            - uint8_t (0 or 1)
 *                      0 → Pin is LOW
 *                      1 → Pin is HIGH
 *
 * @Note              - Reads value from IDR (Input Data Register)
 *********************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
    uint8_t pinState;

    /*****************************************************************
     * Read the logic level from a specific GPIO pin
     *
     * Step 1: Access IDR (Input Data Register)
     * ---------------------------------------------------------------
     * - IDR holds the real-time input values of all 16 GPIO pins
     * - Each pin corresponds to 1 bit:
     *      Pin 0 → bit 0
     *      Pin 1 → bit 1
     *      ...
     *      Pin n → bit n
     *
     * Step 2: Right shift by PinNumber
     * ---------------------------------------------------------------
     * - (pGPIOx->IDR >> PinNumber)
     * - Moves the target pin's bit to LSB (bit 0 position)
     *
     * WHY?
     * - Makes it easier to extract the value using masking
     *
     * Example:
     *   IDR = 0b0010 0000 (Pin 5 is HIGH)
     *   Shift by 5 → 0b0000 0001
     *
     * Step 3: Mask with 0x01
     * ---------------------------------------------------------------
     * - 0x01 = binary 0000 0001 → mask for 1 bit
     * - Extracts only the LSB (target pin value)
     *
     * WHY?
     * - Ensures result is strictly 0 or 1
     * - Removes all other bits
     *
     * Final Result:
     *   0 → Pin is LOW
     *   1 → Pin is HIGH
     *****************************************************************/
    pinState = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);

    return pinState;
}

/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPort
 *
 * @brief             - Read the entire 16-bit input value of a GPIO port
 *
 * @param[in]         - pGPIOx: Base address of the GPIO peripheral
 *
 * @return            - uint16_t (16-bit value representing all GPIO pins)
 *                      Each bit corresponds to a pin:
 *                      bit0 → pin0, bit1 → pin1, ... bit15 → pin15
 *
 * @Note              - Reads directly from IDR (Input Data Register)
 *********************************************************************/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
    uint16_t portValue;

    /* Read IDR (32-bit) and keep only lower 16 bits (GPIO pins 0–15) */
    portValue = (uint16_t)pGPIOx->IDR;

    /* Return the input state of all GPIO pins */
    return portValue;
}

/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPin
 *
 * @brief             - Set or clear a specific GPIO output pin
 *
 * @param[in]         - pGPIOx: Base address of the GPIO peripheral
 * @param[in]         - PinNumber: GPIO pin number (0 to 15)
 * @param[in]         - Value: GPIO_PIN_SET (1) or GPIO_PIN_RESET (0)
 *
 * @return            - None
 *
 * @Note              - Writes to ODR (Output Data Register)
 *********************************************************************/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		/* Set bit → output HIGH */
		pGPIOx->ODR |= (1 << PinNumber);
	} else
	{
		/* Clear bit → output LOW */
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/*********************************************************************
 * @brief  Write a value to the entire GPIO output port
 *
 * @details
 *  - Writes a 16-bit value to the GPIO Output Data Register (ODR)
 *  - Each bit in 'Value' corresponds to one GPIO pin (0–15)
 *      bit 0  → Pin 0
 *      bit 1  → Pin 1
 *      ...
 *      bit 15 → Pin 15
 *
 *  - This operation updates all pins of the port at once
 *
 * @note
 *  - Be careful: This overwrites the entire port (not individual pins)
 *
 *********************************************************************/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx->ODR = Value;
}

/*********************************************************************
 * @brief  Toggle the state of a specific GPIO output pin
 *
 * @details
 *  - Uses XOR operation to invert the current pin state
 *      1 ^ 1 → 0  (HIGH → LOW)
 *      0 ^ 1 → 1  (LOW  → HIGH)
 *
 *  - (1 << PinNumber) creates a mask for the selected pin
 *  - XOR (^) flips only that bit, others remain unchanged
 *
 * @note
 *  - This is a read-modify-write operation (not atomic)
 *  - Not safe in interrupt-heavy applications
 *
 *********************************************************************/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}

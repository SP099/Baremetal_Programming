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

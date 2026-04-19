/*
 * 001LED_Toggle.c
 *
 *  Created on: Apr 19, 2026
 *      Author: Lenovo
 */

#include "stm32f407xx.h"

/*********************************************************************
 * @brief  Simple software delay function
 *
 * @note   This is a blocking delay (busy wait)
 *          - Not accurate (depends on compiler optimization & clock)
 *          - CPU stays in loop doing nothing
 *          - Used only for basic LED blinking demos
 *********************************************************************/
void delay(void)
{
    for(uint32_t i = 0; i < 500000; i++);
}

int main(void)
{
    /*****************************************************************
     * GPIO Handle structure for LED configuration
     * This structure holds:
     *  - GPIO port base address
     *  - GPIO pin configuration parameters
     *****************************************************************/
    GPIO_Handle_t GpioLed;

    /*****************************************************************
     * 1. Select GPIO peripheral (Port D)
     *****************************************************************/
    GpioLed.pGPIOx = GPIOD;

    /*****************************************************************
     * 2. Select GPIO pin number (PD15)
     *    - On STM32F407 Discovery board, LED is connected to PD15
     *****************************************************************/
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;

    /*****************************************************************
     * 3. Configure GPIO mode as Output mode
     *****************************************************************/
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;

    /*****************************************************************
     * 4. Configure output speed (fast switching capability)
     *****************************************************************/
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    /*****************************************************************
     * 5. Configure output type as Push-Pull
     *    - Push-Pull: can drive both HIGH and LOW actively
     *****************************************************************/
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

    /*****************************************************************
     * 6. Disable internal pull-up and pull-down resistors
     *****************************************************************/
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    /*****************************************************************
     * 7. Enable peripheral clock for GPIOD
     *    - Without clock, GPIO registers will not respond
     *****************************************************************/
    GPIO_PeriClockControl(GPIOD, ENABLE);

    /*****************************************************************
     * 8. Initialize GPIO with configured parameters
     *    - Configures MODER, OTYPER, OSPEEDR, PUPDR registers
     *****************************************************************/
    GPIO_Init(&GpioLed);

    /*****************************************************************
     * 9. Infinite loop (main application logic)
     *    - Toggles LED continuously with delay
     *****************************************************************/
    while(1)
    {
        /*************************************************************
         * Toggle LED on PD15
         * - XOR operation flips current pin state
         * - LED ON → OFF → ON → OFF ...
         *************************************************************/
        GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_15);

        /*************************************************************
         * Software delay to make LED blink visible
         *************************************************************/
        delay();
    }

    return 0;
}

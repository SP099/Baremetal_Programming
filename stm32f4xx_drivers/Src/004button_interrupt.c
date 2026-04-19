/*
 * 004button_interrupt.c
 *
 *  Created on: Apr 19, 2026
 *      Author: Lenovo
 */

/*
 * 004button_interrupt.c
 * LED toggle using external interrupt (GPIO button)
 */

#include <string.h>
#include "stm32f407xx.h"

__vo uint8_t button_pressed = 0;

int main(void)
{
    GPIO_Handle_t GpioLed, GpioBtn;

    memset(&GpioLed, 0, sizeof(GpioLed));
    memset(&GpioBtn, 0, sizeof(GpioBtn));

    /*****************************************************************
     * ================= LED CONFIGURATION ===========================
     *****************************************************************/
    GpioLed.pGPIOx = GPIOD;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(GPIOD, ENABLE);
    GPIO_Init(&GpioLed);

    /*****************************************************************
     * ================= BUTTON CONFIGURATION ========================
     * Button connected to PA0 (interrupt mode - Rising edge)
     *****************************************************************/
    GpioBtn.pGPIOx = GPIOA;
    GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
    GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT;
    GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(GPIOA, ENABLE);
    GPIO_Init(&GpioBtn);

    /*****************************************************************
     * ================= NVIC CONFIGURATION =========================
     *****************************************************************/
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(IRQ_NO_EXTI0, ENABLE);

    while(1)
    {
    	if(button_pressed)
    	{
    		for(volatile uint32_t i = 0; i < 500000/2; i++); /* debounce */
    		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
    		button_pressed = 0;
    	}
    }
}

/*********************************************************************
 * ISR: Handles EXTI line 5 to 9 interrupts
 *********************************************************************/
void EXTI0_IRQHandler(void)
{
    GPIO_IRQHandling(GPIO_PIN_NO_0);

    button_pressed = 1;
}

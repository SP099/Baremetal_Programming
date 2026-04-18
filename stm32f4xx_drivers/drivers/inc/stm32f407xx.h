/*
 * stm32f407xx.h
 *
 *  Created on: Apr 18, 2026
 *      Author: Lenovo
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

#define __vo volatile
/*
 * Base addresses of Flash and SRAM memories
 */
#define FLASH_BASEADDR  						0x08000000U  /* Base address of Flash memory (program storage, non-volatile) */
#define SRAM1_BASEADDR  						0x20000000U  /* Base address of SRAM1 (main RAM, 112 KB) */
#define SRAM2_BASEADDR  						0x2001C000U  /* Base of SRAM2 = SRAM1_BASE (0x20000000) + 112KB (0x1C000) */
#define ROM             						0x1FFF0000U  /* Base address of system memory (bootloader ROM) */
#define SRAM            						SRAM1_BASEADDR  /* Alias for SRAM1 base address */

/*
 * AHBx and APBx Bus Peripheral base addresses
 */
#define PERIPH_BASEADDR        					0x40000000U  /* Base address of all peripheral registers */
#define APB1PERIPH_BASEADDR    					PERIPH_BASEADDR  /* Base address of APB1 bus peripherals (low-speed bus) */
#define APB2PERIPH_BASEADDR    					0x40010000U  /* Base address of APB2 bus peripherals (high-speed bus, offset 0x10000 from PERIPH_BASEADDR) */
#define AHB1PERIPH_BASEADDR    					0x40020000U  /* Base address of AHB1 bus peripherals (high-performance bus, offset 0x20000) */
#define AHB2PERIPH_BASEADDR    					0x50000000U  /* Base address of AHB2 bus peripherals (separate high-speed bus region) */

/*
 * Base addresses of peripherals which are hanging on AHB1 bus
 */
#define GPIOA_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x0000U) /* 0x40020000U */
#define GPIOB_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x0400U) /* 0x40020400U */
#define GPIOC_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x0800U) /* 0x40020800U */
#define GPIOD_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x0C00U) /* 0x40020C00U */
#define GPIOE_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x1000U) /* 0x40021000U */
#define GPIOF_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x1400U) /* 0x40021400U */
#define GPIOG_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x1800U) /* 0x40021800U */
#define GPIOH_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x1C00U) /* 0x40021C00U */
#define GPIOI_BASEADDR    						(AHB1PERIPH_BASEADDR + 0x2000U) /* 0x40022000U */

#define RCC_BASEADDR      						(AHB1PERIPH_BASEADDR + 0x3800U) /* 0x40023800U */

/*
 * Base addresses of peripherals which are hanging on APB1 bus
 */
#define I2C1_BASEADDR     						(APB1PERIPH_BASEADDR + 0x5400U) /* 0x40005400U */
#define I2C2_BASEADDR     						(APB1PERIPH_BASEADDR + 0x5800U) /* 0x40005800U */
#define I2C3_BASEADDR     						(APB1PERIPH_BASEADDR + 0x5C00U) /* 0x40005C00U */

#define SPI2_BASEADDR     						(APB1PERIPH_BASEADDR + 0x3800U) /* 0x40003800U */
#define SPI3_BASEADDR    						(APB1PERIPH_BASEADDR + 0x3C00U) /* 0x40003C00U */

#define USART2_BASEADDR   						(APB1PERIPH_BASEADDR + 0x4400U) /* 0x40004400U */
#define USART3_BASEADDR   						(APB1PERIPH_BASEADDR + 0x4800U) /* 0x40004800U */

#define UART4_BASEADDR    						(APB1PERIPH_BASEADDR + 0x4C00U) /* 0x40004C00U */
#define UART5_BASEADDR    						(APB1PERIPH_BASEADDR + 0x5000U) /* 0x40005000U */

/*
 * Base addresses of peripherals which are hanging on APB2 bus
 */
#define EXTI_BASEADDR							(APB2PERIPH_BASEADDR + 0x3C00U) /* 0x40013C00U */
#define SPI1_BASEADDR							(APB2PERIPH_BASEADDR + 0x3000U) /* 0x40013000U */
#define SYSCFG_BASEADDR        					(APB2PERIPH_BASEADDR + 0x3800U) /* 0x40013800U */
#define USART1_BASEADDR							(APB2PERIPH_BASEADDR + 0x1000U) /* 0x40011000U */
#define USART6_BASEADDR							(APB2PERIPH_BASEADDR + 0x1400U) /* 0x40011400U */

/**********************************peripheral register definition structures **********************************/

/*
 * Note : Registers of a peripheral are specific to MCU
 * e.g : Number of Registers of SPI peripheral of STM32F4x family of MCUs may be different(more or less)
 * Compared to number of registers of SPI peripheral of STM32Lx or STM32F0x family of MCUs
 */
typedef struct
{
	__vo uint32_t MODER;    /* Offset: 0x00 - GPIO port mode register
	                         * Each pin has 2 bits (MODERy[1:0])
	                         * 00: Input, 01: Output, 10: Alternate, 11: Analog
	                         */

	__vo uint32_t OTYPER;   /* Offset: 0x04 - GPIO output type register
	                         * Each pin has 1 bit (OTy)
	                         * 0: Push-pull, 1: Open-drain
	                         */

	__vo uint32_t OSPEEDR;  /* Offset: 0x08 - GPIO output speed register
	                         * Each pin has 2 bits (OSPEEDRy[1:0])
	                         * 00: Low, 01: Medium, 10: High, 11: Very high
	                         */

	__vo uint32_t PUPDR;    /* Offset: 0x0C - GPIO pull-up/pull-down register
	                         * Each pin has 2 bits (PUPDRy[1:0])
	                         * 00: No pull, 01: Pull-up, 10: Pull-down
	                         */

	__vo uint32_t IDR;      /* Offset: 0x10 - GPIO input data register
	                         * Read-only input state (IDRy)
	                         * Bit y reflects input level of pin y
	                         */

	__vo uint32_t ODR;      /* Offset: 0x14 - GPIO output data register
	                         * Write/read output state (ODRy)
	                         * 1 = High, 0 = Low
	                         */

	__vo uint32_t BSRR;     /* Offset: 0x18 - GPIO bit set/reset register
	                         * Lower 16 bits: Set pin (BSy)
	                         * Upper 16 bits: Reset pin (BRy)
	                         * Atomic set/reset operation
	                         */

	__vo uint32_t LCKR;     /* Offset: 0x1C - GPIO configuration lock register
	                         * Locks GPIO configuration until reset
	                         * LCKKy bit sequence required
	                         */

	__vo uint32_t AFR[2];   /* Offset: 0x20 - Alternate function registers
	                         * AFR[0] = AFRL (pins 0–7)
	                         * AFR[1] = AFRH (pins 8–15)
	                         * Each pin uses 4 bits (AFRy[3:0])
	                         */
} GPIO_RegDef_t;


/*
 * peripheral definitions ( Peripheral base addresses typecasted to xxx_RegDef_t)
 */
#define GPIOA   ((GPIO_RegDef_t*)GPIOA_BASEADDR)  /* GPIO Port A base pointer */
#define GPIOB   ((GPIO_RegDef_t*)GPIOB_BASEADDR)  /* GPIO Port B base pointer */
#define GPIOC   ((GPIO_RegDef_t*)GPIOC_BASEADDR)  /* GPIO Port C base pointer */
#define GPIOD   ((GPIO_RegDef_t*)GPIOD_BASEADDR)  /* GPIO Port D base pointer */
#define GPIOE   ((GPIO_RegDef_t*)GPIOE_BASEADDR)  /* GPIO Port E base pointer */
#define GPIOF   ((GPIO_RegDef_t*)GPIOF_BASEADDR)  /* GPIO Port F base pointer */
#define GPIOG   ((GPIO_RegDef_t*)GPIOG_BASEADDR)  /* GPIO Port G base pointer */
#define GPIOH   ((GPIO_RegDef_t*)GPIOH_BASEADDR)  /* GPIO Port H base pointer */
#define GPIOI   ((GPIO_RegDef_t*)GPIOI_BASEADDR)  /* GPIO Port I base pointer */

#endif /* INC_STM32F407XX_H_ */

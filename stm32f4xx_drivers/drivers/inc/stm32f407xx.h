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
 * peripheral register definition structure for RCC
 */
typedef struct
{
	__vo uint32_t CR;         /* Offset: 0x00 - Clock control register
	                          * Controls internal/external oscillators (HSI, HSE, PLL)
	                          * Enables/disables clocks and indicates ready flags
	                          */

	__vo uint32_t PLLCFGR;    /* Offset: 0x04 - PLL configuration register
	                          * Configures PLL source, multiplication and division factors
	                          */

	__vo uint32_t CFGR;       /* Offset: 0x08 - Clock configuration register
	                          * Selects system clock source and prescalers for AHB, APB1, APB2
	                          */

	__vo uint32_t CIR;        /* Offset: 0x0C - Clock interrupt register
	                          * Handles clock ready interrupts and flags
	                          */

	__vo uint32_t AHB1RSTR;   /* Offset: 0x10 - AHB1 peripheral reset register
	                          * Resets peripherals on AHB1 bus
	                          */

	__vo uint32_t AHB2RSTR;   /* Offset: 0x14 - AHB2 peripheral reset register
	                          * Resets peripherals on AHB2 bus
	                          */

	__vo uint32_t AHB3RSTR;   /* Offset: 0x18 - AHB3 peripheral reset register
	                          * Resets peripherals on AHB3 bus
	                          */

	uint32_t      RESERVED0;  /* Offset: 0x1C - Reserved */

	__vo uint32_t APB1RSTR;   /* Offset: 0x20 - APB1 peripheral reset register
	                          * Resets peripherals on APB1 bus
	                          */

	__vo uint32_t APB2RSTR;   /* Offset: 0x24 - APB2 peripheral reset register
	                          * Resets peripherals on APB2 bus
	                          */

	uint32_t      RESERVED1[2]; /* Offset: 0x28-0x2C - Reserved */

	__vo uint32_t AHB1ENR;    /* Offset: 0x30 - AHB1 peripheral clock enable register
	                          * Enables clock for AHB1 peripherals
	                          */

	__vo uint32_t AHB2ENR;    /* Offset: 0x34 - AHB2 peripheral clock enable register
	                          * Enables clock for AHB2 peripherals
	                          */

	__vo uint32_t AHB3ENR;    /* Offset: 0x38 - AHB3 peripheral clock enable register
	                          * Enables clock for AHB3 peripherals
	                          */

	uint32_t      RESERVED2;  /* Offset: 0x3C - Reserved */

	__vo uint32_t APB1ENR;    /* Offset: 0x40 - APB1 peripheral clock enable register
	                          * Enables clock for APB1 peripherals
	                          */

	__vo uint32_t APB2ENR;    /* Offset: 0x44 - APB2 peripheral clock enable register
	                          * Enables clock for APB2 peripherals
	                          */

	uint32_t      RESERVED3[2]; /* Offset: 0x48-0x4C - Reserved */

	__vo uint32_t AHB1LPENR;  /* Offset: 0x50 - AHB1 clock enable in low power mode
	                          * Enables/disables AHB1 peripheral clocks in Sleep mode
	                          */

	__vo uint32_t AHB2LPENR;  /* Offset: 0x54 - AHB2 clock enable in low power mode
	                          */

	__vo uint32_t AHB3LPENR;  /* Offset: 0x58 - AHB3 clock enable in low power mode
	                          */

	uint32_t      RESERVED4;  /* Offset: 0x5C - Reserved */

	__vo uint32_t APB1LPENR;  /* Offset: 0x60 - APB1 clock enable in low power mode
	                          */

	__vo uint32_t APB2LPENR;  /* Offset: 0x64 - APB2 clock enable in low power mode
	                          */

	uint32_t      RESERVED5[2]; /* Offset: 0x68-0x6C - Reserved */

	__vo uint32_t BDCR;       /* Offset: 0x70 - Backup domain control register
	                          * Controls RTC clock source and backup domain reset
	                          */

	__vo uint32_t CSR;        /* Offset: 0x74 - Clock control & status register
	                          * Provides reset flags and low-speed clock control
	                          */

	uint32_t      RESERVED6[2]; /* Offset: 0x78-0x7C - Reserved */

	__vo uint32_t SSCGR;      /* Offset: 0x80 - Spread spectrum clock generation register
	                          * Used for EMI reduction via clock modulation
	                          */

	__vo uint32_t PLLI2SCFGR; /* Offset: 0x84 - PLLI2S configuration register
	                          * Configures PLLI2S for I2S peripheral clocks
	                          */

	__vo uint32_t PLLSAICFGR; /* Offset: 0x88 - PLLSAI configuration register
	                          * Configures PLLSAI for LCD, SAI, etc.
	                          */

	__vo uint32_t DCKCFGR;    /* Offset: 0x8C - Dedicated clock configuration register
	                          * Selects clock sources for specific peripherals
	                          */

	__vo uint32_t CKGATENR;   /* Offset: 0x90 - Clocks gated enable register
	                          * Controls clock gating for peripherals
	                          */

	__vo uint32_t DCKCFGR2;   /* Offset: 0x94 - Dedicated clock configuration register 2
	                          * Additional clock configuration options
	                          */

} RCC_RegDef_t;

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

#define RCC 	((RCC_RegDeg_t*)RCC_BASEADDR)     /* Base pointer for RCC */
/*
 * Clock Enable Macros for GPIOx peripherals
 * Register: RCC_AHB1ENR
 * Logic:
 *  - Go to Reference Manual → RCC → AHB1ENR register
 *  - Each GPIO port has a fixed bit position
 *  - Enable = set that bit to 1 → (1 << bit_position)
 */

#define GPIOA_PCLK_EN() (RCC->AHB1ENR |= (1 << 0))  /* Bit 0  -> GPIOA clock enable */
#define GPIOB_PCLK_EN() (RCC->AHB1ENR |= (1 << 1))  /* Bit 1  -> GPIOB clock enable */
#define GPIOC_PCLK_EN() (RCC->AHB1ENR |= (1 << 2))  /* Bit 2  -> GPIOC clock enable */
#define GPIOD_PCLK_EN() (RCC->AHB1ENR |= (1 << 3))  /* Bit 3  -> GPIOD clock enable */
#define GPIOE_PCLK_EN() (RCC->AHB1ENR |= (1 << 4))  /* Bit 4  -> GPIOE clock enable */
#define GPIOF_PCLK_EN() (RCC->AHB1ENR |= (1 << 5))  /* Bit 5  -> GPIOF clock enable */
#define GPIOG_PCLK_EN() (RCC->AHB1ENR |= (1 << 6))  /* Bit 6  -> GPIOG clock enable */
#define GPIOH_PCLK_EN() (RCC->AHB1ENR |= (1 << 7))  /* Bit 7  -> GPIOH clock enable */
#define GPIOI_PCLK_EN() (RCC->AHB1ENR |= (1 << 8))  /* Bit 8  -> GPIOI clock enable */


/*
 * Clock Enable Macros for I2Cx peripherals
 * Register: RCC_APB1ENR
 * Logic:
 *  - I2C peripherals are on APB1 bus
 *  - Check APB1ENR register bit mapping in RM
 */

#define I2C1_PCLK_EN() (RCC->APB1ENR |= (1 << 21))  /* Bit 21 -> I2C1 clock enable */
#define I2C2_PCLK_EN() (RCC->APB1ENR |= (1 << 22))  /* Bit 22 -> I2C2 clock enable */
#define I2C3_PCLK_EN() (RCC->APB1ENR |= (1 << 23))  /* Bit 23 -> I2C3 clock enable */


/*
 * Clock Enable Macros for SPIx peripherals
 * Logic:
 *  - SPI1 & SPI4 → APB2 bus
 *  - SPI2 & SPI3 → APB1 bus
 *  - Bit positions taken from respective ENR registers
 */

#define SPI1_PCLK_EN() (RCC->APB2ENR |= (1 << 12))  /* APB2 Bit 12 -> SPI1 */
#define SPI2_PCLK_EN() (RCC->APB1ENR |= (1 << 14))  /* APB1 Bit 14 -> SPI2 */
#define SPI3_PCLK_EN() (RCC->APB1ENR |= (1 << 15))  /* APB1 Bit 15 -> SPI3 */
#define SPI4_PCLK_EN() (RCC->APB2ENR |= (1 << 13))  /* APB2 Bit 13 -> SPI4 */


/*
 * Clock Enable Macros for USARTx/UARTx peripherals
 * Logic:
 *  - USART1 & USART6 → APB2
 *  - USART2,3, UART4,5 → APB1
 *  - Bit positions from RM tables
 */

#define USART1_PCCK_EN() (RCC->APB2ENR |= (1 << 4))   /* APB2 Bit 4  -> USART1 */
#define USART2_PCCK_EN() (RCC->APB1ENR |= (1 << 17))  /* APB1 Bit 17 -> USART2 */
#define USART3_PCCK_EN() (RCC->APB1ENR |= (1 << 18))  /* APB1 Bit 18 -> USART3 */
#define UART4_PCCK_EN()  (RCC->APB1ENR |= (1 << 19))  /* APB1 Bit 19 -> UART4 */
#define UART5_PCCK_EN()  (RCC->APB1ENR |= (1 << 20))  /* APB1 Bit 20 -> UART5 */
#define USART6_PCCK_EN() (RCC->APB2ENR |= (1 << 5))   /* APB2 Bit 5  -> USART6 (IMPORTANT FIX) */


/*
 * Clock Enable Macro for SYSCFG
 * Register: RCC_APB2ENR
 */

#define SYSCFG_PCLK_EN() (RCC->APB2ENR |= (1 << 14)) /* Bit 14 -> SYSCFG */


/*
 * Clock Disable Macros
 * Logic:
 *  - Disable = clear bit → AND with NOT mask
 *  - ~(1 << bit_position)
 */

#define GPIOA_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 0))  /* Clear Bit 0 -> Disable GPIOA */
#define GPIOB_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 1))
#define GPIOC_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 2))
#define GPIOD_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 3))
#define GPIOE_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 4))
#define GPIOF_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 5))
#define GPIOG_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 6))
#define GPIOH_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 7))
#define GPIOI_PCLK_DI() (RCC->AHB1ENR &= ~(1 << 8))

#define I2C1_PCLK_DI() (RCC->APB1ENR &= ~(1 << 21))
#define I2C2_PCLK_DI() (RCC->APB1ENR &= ~(1 << 22))
#define I2C3_PCLK_DI() (RCC->APB1ENR &= ~(1 << 23))

#define SPI1_PCLK_DI() (RCC->APB2ENR &= ~(1 << 12))
#define SPI2_PCLK_DI() (RCC->APB1ENR &= ~(1 << 14))
#define SPI3_PCLK_DI() (RCC->APB1ENR &= ~(1 << 15))
#define SPI4_PCLK_DI() (RCC->APB2ENR &= ~(1 << 13))

#define USART1_PCLK_DI() (RCC->APB2ENR &= ~(1 << 4))
#define USART2_PCLK_DI() (RCC->APB1ENR &= ~(1 << 17))
#define USART3_PCLK_DI() (RCC->APB1ENR &= ~(1 << 18))
#define UART4_PCLK_DI()  (RCC->APB1ENR &= ~(1 << 19))
#define UART5_PCLK_DI()  (RCC->APB1ENR &= ~(1 << 20))
#define USART6_PCLK_DI() (RCC->APB2ENR &= ~(1 << 5))

#define SYSCFG_PCLK_DI() (RCC->APB2ENR &= ~(1 << 14))
#endif /* INC_STM32F407XX_H_ */

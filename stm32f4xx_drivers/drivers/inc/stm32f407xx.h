/*
 * stm32f407xx.h
 *
 *  Created on: Apr 18, 2026
 *      Author: Lenovo
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

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

#endif /* INC_STM32F407XX_H_ */

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


#endif /* INC_STM32F407XX_H_ */

/*
 * stm32f407xx_i2c_driver.h
 *
 *  Created on: Apr 22, 2026
 *      Author: Lenovo
 */

#ifndef INC_STM32F407XX_I2C_DRIVER_H_
#define INC_STM32F407XX_I2C_DRIVER_H_

#include "stm32f407xx.h"

/*
 * @I2C_Config_t
 * @brief  Configuration structure for I2C peripheral
 *
 * @details
 *  This structure holds all user-configurable parameters
 *  required to initialize the I2C peripheral.
 *
 * @members
 *  I2C_SCLSpeed     : Clock speed (Standard/Fast mode)
 *  I2C_DeviceAddress: Device own address
 *  I2C_ACKControl   : Enable/Disable ACK
 *  I2C_FMDutyCycle  : Fast mode duty cycle
 */
typedef struct
{
    uint32_t I2C_SCLSpeed;
    uint8_t  I2C_DeviceAddress;
    uint8_t  I2C_ACKContol;
    uint16_t I2C_FMDutyCycle;

} I2C_Config_t;

/*
 * @I2C_Handle_t
 * @brief  Handle structure for I2C peripheral
 *
 * @details
 *  This structure is used to manage I2C operations.
 *  It contains:
 *   - Pointer to I2C peripheral base address
 *   - Configuration structure
 *
 *  Used in all I2C driver APIs.
 */
typedef struct
{
    I2C_RegDef_t *pI2Cx;   /* Base address of I2C peripheral */
    I2C_Config_t I2C_Config; /* I2C configuration settings */

} I2C_Handle_t;

/*
 * @I2C_SCLSpeed
 * @brief  Possible I2C clock speeds
 */
#define I2C_SCL_SPEED_SM     100000   /* Standard mode (100kHz) */
#define I2C_SCL_SPEED_FM2K   200000   /* Fast mode (200kHz) */
#define I2C_SCL_SPEED_FM4K   400000   /* Fast mode (400kHz) */

/*
 * @I2C_ACKControl
 * @brief  Enable/Disable ACK bit
 */
#define I2C_ACK_ENABLE       1
#define I2C_ACK_DISABLE      0

/*
 * @I2C_FMDutyCycle
 * @brief  Fast mode duty cycle configuration
 */
#define I2C_FM_DUTY_2        0   /* Tlow/Thigh = 2 */
#define I2C_FM_DUTY_16_9     1   /* Tlow/Thigh = 16/9 */

/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/
/*
 * Peripheral Clock setup
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);

/*
 * IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);

/*
 * Other Peripheral Control APIs
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName);

/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv);
#endif /* INC_STM32F407XX_I2C_DRIVER_H_ */

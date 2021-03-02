/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIO_RA_H
#define GPIO_RA_H

#include "r_ioport.h"

/*!
 */
void Gpio_Ra_Init(void);

/*!
 * @param pin
 * @param configuration
 */
void Gpio_Ra_ConfigurePin(bsp_io_port_pin_t pin, uint32_t configuration);

/*!
 * @param pin
 * @return true
 * @return
 */
bool Gpio_Ra_Read(bsp_io_port_pin_t pin);

/*!
 * @param pin
 * @param state
 */
void Gpio_Ra_Write(bsp_io_port_pin_t pin, bool state);

#endif

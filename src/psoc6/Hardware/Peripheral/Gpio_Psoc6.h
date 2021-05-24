/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GPIO_PSOC6_H
#define GPIO_PSOC6_H

#include "cy_pdl.h"

/*!
 * @param port
 * @param pin
 * @param config
 */
void Gpio_Psoc6_ConfigurePin(uint8_t port, uint8_t pin, const cy_stc_gpio_pin_config_t *config);

/*!
 * @param port
 * @param pin
 * @return
 */
bool Gpio_Psoc6_Read(uint8_t port, uint8_t pin);

/*!
 * @param port
 * @param pin
 * @param state
 */
void Gpio_Psoc6_Write(uint8_t port, uint8_t pin, bool state);

#endif

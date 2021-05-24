/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Gpio_Psoc6.h"
#include "uassert.h"

void Gpio_Psoc6_ConfigurePin(uint8_t port, uint8_t pin, const cy_stc_gpio_pin_config_t *config)
{
   cy_en_gpio_status_t status = Cy_GPIO_Pin_Init(Cy_GPIO_PortToAddr(port), pin, config);
   uassert(status == CY_GPIO_SUCCESS);
}

bool Gpio_Psoc6_Read(uint8_t port, uint8_t pin)
{
   return (bool)Cy_GPIO_Read(Cy_GPIO_PortToAddr(port), pin);
}

void Gpio_Psoc6_Write(uint8_t port, uint8_t pin, bool state)
{
   Cy_GPIO_Write(Cy_GPIO_PortToAddr(port), pin, (uint32_t)state);
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Gpio_Ra.h"
#include "uassert.h"

static ioport_instance_ctrl_t gpioControl;

static const ioport_pin_cfg_t pinConfig;

// Individual modules will initialize their own pins via R_IOPORT_PinCfg(),
// however we still need to supply an empty initial configuration to R_IOPORT_Open()
const ioport_cfg_t gpioConfig = {
   .number_of_pins = 0,
   .p_pin_cfg_data = &pinConfig,
};

void Gpio_Ra_Init(void)
{
   fsp_err_t status = R_IOPORT_Open(&gpioControl, &gpioConfig);
   uassert(status == FSP_SUCCESS);
}

void Gpio_Ra_ConfigurePin(bsp_io_port_pin_t pin, uint32_t configuration)
{
   fsp_err_t status = R_IOPORT_PinCfg(&gpioControl, pin, configuration);
   uassert(status == FSP_SUCCESS);
}

bool Gpio_Ra_Read(bsp_io_port_pin_t pin)
{
   bool state;
   fsp_err_t status = R_IOPORT_PinRead(&gpioControl, pin, (bsp_io_level_t *)&state);
   uassert(status == FSP_SUCCESS);

   return state;
}

void Gpio_Ra_Write(bsp_io_port_pin_t pin, bool state)
{
   fsp_err_t status = R_IOPORT_PinWrite(&gpioControl, pin, (bsp_io_level_t)state);
   uassert(status == FSP_SUCCESS);
}

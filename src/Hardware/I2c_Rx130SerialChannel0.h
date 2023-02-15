/*!
 * @file
 * @brief Implementation of the the I2C driver on the SCI0 peripheral
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef I2C_RX130SERIALCHANNEL5_H
#define I2C_RX130SERIALCHANNEL5_H

#include "I_I2c.h"
#include "I_ContextProtector.h"

/*!
 * Initialize Serial Channel 0
 */
I_I2c_t *I2c_Rx130SerialChannel0_Init(I_ContextProtector_t *contextProtector);

#endif

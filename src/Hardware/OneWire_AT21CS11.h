/*!
 * @file
 * @brief One wire AT21CS11 EEPROM driver
 * Note that this code bit-bangs the one wire interface, so turns off interrupts during transfers.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ONEWIRE_AT21CS11_H
#define ONEWIRE_AT21CS11_H

#include "I_OneWire.h"

/*!
 * Start up the one wire driver
 * @return Pointer to device driver
 */
I_OneWire_t *OneWire_AT21CS11_Init(void);

#endif

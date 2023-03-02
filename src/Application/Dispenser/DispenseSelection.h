/*!
 * @file
 * @brief This file contains dispense selection data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSESELECTION_H
#define DISPENSESELECTION_H

#include <stdint.h>

enum
{
   DispenseSelection_ColdWater,
   DispenseSelection_CrushedIce,
   DispenseSelection_CubedIce,
   DispenseSelection_Reset = UINT8_MAX
};
typedef uint8_t DispenseSelection_t;

#endif

/*!
 * @file
 * @brief Ice Cabinet Cooling Modes
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETCOOLINGMODE_H
#define ICECABINETCOOLINGMODE_H

#include <stdint.h>

enum
{
   IceCabinetCoolingMode_Neutral,
   IceCabinetCoolingMode_ForcedCooling,
   IceCabinetCoolingMode_ActiveCooling,
   IceCabinetCoolingMode_FallbackCooling,
};
typedef uint8_t IceCabinetCoolingMode_t;

#endif

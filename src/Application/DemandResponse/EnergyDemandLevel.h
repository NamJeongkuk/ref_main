/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENERGYDEMANDLEVEL_H
#define ENERGYDEMANDLEVEL_H

#include <stdint.h>

enum
{
   EnergyDemandLevel_Off,
   EnergyDemandLevel_Low,
   EnergyDemandLevel_MaxLoadReduction,
   EnergyDemandLevel_DelayDefrost
};
typedef uint8_t EnergyDemandLevel_t;

#endif

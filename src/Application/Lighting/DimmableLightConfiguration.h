/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DIMMABLELIGHTCONFIGURATION_H
#define DIMMABLELIGHTCONFIGURATION_H

#include <stdint.h>

enum
{
   DimmableLightConfiguration_FreshFoodBackLight = 0,
   DimmableLightConfiguration_FreshFoodAllLights,
   DimmableLightConfiguration_FreezerBackLight,
   DimmableLightConfiguration_FreezerAllLights,
   DimmableLightConfiguration_FreshFoodAndFreezerBackLight,
   DimmableLightConfiguration_FreshFoodAndFreezerAllLights,
};
typedef uint8_t DimmableLightConfiguration_t;

#endif

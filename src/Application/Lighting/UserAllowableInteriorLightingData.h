/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERALLOWABLEINTERIORLIGHTINGDATA_H
#define USERALLOWABLEINTERIORLIGHTINGDATA_H

#include <stdint.h>

enum
{
   UserAllowableInteriorLighting_FreshFoodBackLight,
   UserAllowableInteriorLighting_FreshFoodTopLight,
   UserAllowableInteriorLighting_FreezerBackLight,
   UserAllowableInteriorLighting_FreezerTopLight
};
typedef uint8_t UserAllowableInteriorLighting_t; // Must match interior_light_enum in parametric

typedef struct
{
   uint8_t bitmap[1];
} UserAllowableInteriorLightingBitmap_t;

typedef struct
{
   UserAllowableInteriorLightingBitmap_t userAllowable0InteriorLightingBitmap;
   UserAllowableInteriorLightingBitmap_t userAllowable1InteriorLightingBitmap;
   UserAllowableInteriorLightingBitmap_t userAllowable2InteriorLightingBitmap;
} UserAllowableInteriorLightingData_t;

#endif

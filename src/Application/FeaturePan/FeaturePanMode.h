/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANMODE_H
#define FEATUREPANMODE_H

#include "TemperatureDegFx100.h"
#include "utils.h"

enum
{
   FeaturePanMode_NumberOfFeaturePanModes = 7
};

enum
{
   FeaturePanCoolingMode_Neutral,
   FeaturePanCoolingMode_ActiveCooling,
   FeaturePanCoolingMode_ActiveHeating,
   FeaturePanCoolingMode_ForcedHeating
};
typedef uint8_t FeaturePanCoolingMode_t;

enum
{
   FeaturePanForcedHeatingMode_Inactive,
   FeaturePanForcedHeatingMode_Active,
   FeaturePanForcedHeatingMode_Max
};
typedef uint8_t FeaturePanForcedHeatingMode_t;

enum
{
   FeaturePanCurrentMode_Off,
   FeaturePanCurrentMode_Mode1,
   FeaturePanCurrentMode_Mode2,
   FeaturePanCurrentMode_Mode3,
   FeaturePanCurrentMode_Mode4,
   FeaturePanCurrentMode_Mode5,
   FeaturePanCurrentMode_Mode6,
   FeaturePanCurrentMode_Mode7,
   FeaturePanCurrentMode_NotControllable = UINT8_MAX
};
typedef uint8_t FeaturePanCurrentMode_t;

enum
{
   FeaturePanModeLabel_Freezer,
   FeaturePanModeLabel_Refrigerator,
   FeaturePanModeLabel_Chiller,
   FeaturePanModeLabel_SoftFreeze,
   FeaturePanModeLabel_Meat,
   FeaturePanModeLabel_Beverages,
   FeaturePanModeLabel_Snacks,
   FeaturePanModeLabel_Wine,
   FeaturePanModeLabel_IceCream,
   FeaturePanModeLabel_Drinks,
   FeaturePanModeLabel_Produce,
   FeaturePanModeLabel_Invalid = UINT8_MAX
};
typedef uint8_t FeaturePanModeLabel_t;

typedef struct
{
   TemperatureDegFx100_t minTemperature; // TemperatureDegFx100_t
   TemperatureDegFx100_t maxTemperature; // TemperatureDegFx100_t
   TemperatureDegFx100_t defaultTemperature; // TemperatureDegFx100_t
   FeaturePanModeLabel_t mode; // FeaturePanModeLabel_t
   uint8_t reserved[1];
} FeaturePanModeRange_t;

#define SWAP_DEFINITION_FeaturePanModeRange_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, minTemperature),             \
      SWAPPED_FIELD(_name, _type, maxTemperature),          \
      SWAPPED_FIELD(_name, _type, defaultTemperature),

#endif

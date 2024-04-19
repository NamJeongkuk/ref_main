/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConstArrayMap_FeaturePan.h"

// clang-format off
static const FeaturePanStatesTable_t table[] =
{   //Feature Pan Mode                      Damper Position        Heater State     Fan Speed
   { FeaturePanCoolingMode_Neutral,       { DamperPosition_Closed, HeaterState_Off, FanSpeed_Off }},
   { FeaturePanCoolingMode_ActiveCooling, { DamperPosition_Open,   HeaterState_Off, FanSpeed_On }},
   { FeaturePanCoolingMode_ActiveHeating, { DamperPosition_Closed, HeaterState_On,  FanSpeed_On }},
   { FeaturePanCoolingMode_ForcedHeating, { DamperPosition_Closed, HeaterState_On,  FanSpeed_On }},
};
// clang-format on

static ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig = {
   .array = table,
   .elementCount = ELEMENT_COUNT(table),
   .elementSize = ELEMENT_SIZE(table),
   .keySize = sizeof(FeaturePanCoolingMode_t),
   .keyOffset = OFFSET_OF(FeaturePanStatesTable_t, featurePanCoolingModeKey)
};

I_ConstArrayMap_t *ConstArrayMap_FeaturePan_Init(ConstArrayMap_FeaturePan_t *instance)
{
   ConstArrayMap_LinearSearch_Init(&instance->_private.constArrayMap, &constArrayMapConfig);
   return &instance->_private.constArrayMap.interface;
}

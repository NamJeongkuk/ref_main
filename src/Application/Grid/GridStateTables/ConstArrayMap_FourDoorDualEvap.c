/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConstArrayMap_FourDoorDualEvap.h"

// clang-format off
static const FourDoorDualEvaporatorStatesTable_t table[] =
{    //Cooling Speed         Cooling Mode               Compressor                Condenser Fan       Freezer Evap Fan   Fresh Food Evap Fan  Sealed System Valve Position
   {{ CoolingSpeed_Off,      CoolingMode_Off       }, { CompressorSpeed_Off,      FanSpeed_Off,       FanSpeed_Off,      FanSpeed_Off,        SealedSystemValvePosition_D }},
   {{ CoolingSpeed_Off,      CoolingMode_Freezer   }, { CompressorSpeed_Off,      FanSpeed_Off,       FanSpeed_Off,      FanSpeed_Off,        SealedSystemValvePosition_B }},
   {{ CoolingSpeed_Low,      CoolingMode_Freezer   }, { CompressorSpeed_Low,      FanSpeed_Low,       FanSpeed_Low,      FanSpeed_Off,        SealedSystemValvePosition_B }},
   {{ CoolingSpeed_Mid,      CoolingMode_Freezer   }, { CompressorSpeed_Medium,   FanSpeed_Medium,    FanSpeed_Medium,   FanSpeed_Off,        SealedSystemValvePosition_B }},
   {{ CoolingSpeed_High,     CoolingMode_Freezer   }, { CompressorSpeed_High,     FanSpeed_High,      FanSpeed_High,     FanSpeed_Off,        SealedSystemValvePosition_B }},
   {{ CoolingSpeed_Off,      CoolingMode_FreshFood }, { CompressorSpeed_Off,      FanSpeed_Off,       FanSpeed_Off,      FanSpeed_Off,        SealedSystemValvePosition_A }},
   {{ CoolingSpeed_Low,      CoolingMode_FreshFood }, { CompressorSpeed_Low,      FanSpeed_Low,       FanSpeed_Low,      FanSpeed_Low,        SealedSystemValvePosition_A }},
   {{ CoolingSpeed_Mid,      CoolingMode_FreshFood }, { CompressorSpeed_Medium,   FanSpeed_Medium,    FanSpeed_Medium,   FanSpeed_Medium,     SealedSystemValvePosition_A }},
   {{ CoolingSpeed_High,     CoolingMode_FreshFood }, { CompressorSpeed_High,     FanSpeed_High,      FanSpeed_High,     FanSpeed_High,       SealedSystemValvePosition_A }},
   {{ CoolingSpeed_PullDown, CoolingMode_Freezer   }, { CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low,      FanSpeed_Off,        SealedSystemValvePosition_B }},
   {{ CoolingSpeed_PullDown, CoolingMode_FreshFood }, { CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low,      FanSpeed_Low,        SealedSystemValvePosition_A }}
};
// clang-format on

static ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig = {
   .array = table,
   .elementCount = ELEMENT_COUNT(table),
   .elementSize = ELEMENT_SIZE(table),
   .keySize = sizeof(CoolingMapKey_t),
   .keyOffset = OFFSET_OF(FourDoorDualEvaporatorStatesTable_t, coolingKey)
};

I_ConstArrayMap_t *ConstArrayMap_FourDoorDualEvap_Init(ConstArrayMap_FourDoorDualEvap_t *instance)
{
   ConstArrayMap_LinearSearch_Init(
      &instance->_private.constArrayMap,
      &constArrayMapConfig);

   return &instance->_private.constArrayMap.interface;
}

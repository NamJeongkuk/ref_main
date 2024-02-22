/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConstArrayMap_SideBySideSingleEvap.h"

// clang-format off
static const SideBySideSingleEvaporatorStatesTable_t table[] =
{    //Cooling Speed         Cooling Mode               Compressor              Condenser Fan       Freezer Evap     Fan Fresh Food Damper
   {{ CoolingSpeed_Off,      CoolingMode_Freezer   }, { CompressorSpeed_Off,    FanSpeed_Off,       FanSpeed_Off,    DamperPosition_Closed }},
   {{ CoolingSpeed_Low,      CoolingMode_Freezer   }, { CompressorSpeed_Low,    FanSpeed_Low,       FanSpeed_Low,    DamperPosition_Closed }},
   {{ CoolingSpeed_Mid,      CoolingMode_Freezer   }, { CompressorSpeed_Medium, FanSpeed_Medium,    FanSpeed_Medium, DamperPosition_Closed }},
   {{ CoolingSpeed_High,     CoolingMode_Freezer   }, { CompressorSpeed_High,   FanSpeed_High,      FanSpeed_High,   DamperPosition_Closed }},
   {{ CoolingSpeed_Off,      CoolingMode_FreshFood }, { CompressorSpeed_Off,    FanSpeed_Off,       FanSpeed_Low,    DamperPosition_Open   }},
   {{ CoolingSpeed_Low,      CoolingMode_FreshFood }, { CompressorSpeed_Low,    FanSpeed_Low,       FanSpeed_Low,    DamperPosition_Open   }},
   {{ CoolingSpeed_Mid,      CoolingMode_FreshFood }, { CompressorSpeed_Medium, FanSpeed_Medium,    FanSpeed_Medium, DamperPosition_Open   }},
   {{ CoolingSpeed_High,     CoolingMode_FreshFood }, { CompressorSpeed_High,   FanSpeed_High,      FanSpeed_High,   DamperPosition_Open   }},
   {{ CoolingSpeed_PullDown, CoolingMode_Freezer   }, { CompressorSpeed_Low,    FanSpeed_SuperHigh, FanSpeed_Low,    DamperPosition_Closed }},
   {{ CoolingSpeed_PullDown, CoolingMode_FreshFood }, { CompressorSpeed_Low,    FanSpeed_SuperHigh, FanSpeed_Low,    DamperPosition_Open   }}
};
// clang-format on

static ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig = {
   .array = table,
   .elementCount = ELEMENT_COUNT(table),
   .elementSize = ELEMENT_SIZE(table),
   .keySize = sizeof(CoolingMapKey_t),
   .keyOffset = OFFSET_OF(SideBySideSingleEvaporatorStatesTable_t, coolingKey)
};

I_ConstArrayMap_t *ConstArrayMap_SideBySideSingleEvap_Init(ConstArrayMap_SideBySideSingleEvap_t *instance)
{
   ConstArrayMap_LinearSearch_Init(
      &instance->_private.constArrayMap,
      &constArrayMapConfig);

   return &instance->_private.constArrayMap.interface;
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConstArrayMap_SingleDoorSingleEvap.h"
#include "utils.h"

// clang-format off
static const SingleDoorSingleEvaporatorStatesTable_t table[] =
{    //Cooling Speed              Compressor            Condenser Fan       Primary Evaporator Fan
   { CoolingSpeed_Off,      { CompressorSpeed_Off,      FanSpeed_Off,       FanSpeed_Off }},
   { CoolingSpeed_Low,      { CompressorSpeed_Low,      FanSpeed_Low,       FanSpeed_Low }},
   { CoolingSpeed_Mid,      { CompressorSpeed_Medium,   FanSpeed_Medium,    FanSpeed_Medium }},
   { CoolingSpeed_High,     { CompressorSpeed_High,     FanSpeed_High,      FanSpeed_High }},
   { CoolingSpeed_PullDown, { CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low }}
};
// clang-format on

static ConstArrayMap_LinearSearchConfiguration_t constArrayMapConfig = {
   .array = table,
   .elementCount = ELEMENT_COUNT(table),
   .elementSize = ELEMENT_SIZE(table),
   .keySize = sizeof(CoolingSpeed_t),
   .keyOffset = OFFSET_OF(SingleDoorSingleEvaporatorStatesTable_t, speed)
};

I_ConstArrayMap_t *ConstArrayMap_SingleDoorSingleEvap_Init(ConstArrayMap_SingleDoorSingleEvap_t *instance)
{
   ConstArrayMap_LinearSearch_Init(
      &instance->_private.constArrayMap,
      &constArrayMapConfig);

   return &instance->_private.constArrayMap.interface;
}

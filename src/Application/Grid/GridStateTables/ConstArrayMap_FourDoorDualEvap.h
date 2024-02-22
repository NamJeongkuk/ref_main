/*!
 * @file
 * @brief Four Door specific state based grid voting table.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONSTARRAYMAP_FOURDOORDUALEVAP_H
#define CONSTARRAYMAP_FOURDOORDUALEVAP_H

#include "FanSpeed.h"
#include "CompressorVotedSpeed.h"
#include "SealedSystemValvePosition.h"
#include "I_ConstArrayMap.h"
#include "CoolingMapKey.h"
#include "ConstArrayMap_LinearSearch.h"

typedef struct
{
   struct
   {
      ConstArrayMap_LinearSearch_t constArrayMap;
   } _private;
} ConstArrayMap_FourDoorDualEvap_t;

typedef struct
{
   CompressorSpeed_t compressorSpeed;
   FanSpeed_t condenserFanSpeed;
   FanSpeed_t freezerEvapFanSpeed;
   FanSpeed_t freshFoodEvapFanSpeed;
   SealedSystemValvePosition_t sealedSystemValvePosition;
} FourDoorDualEvaporatorVotes_t;

typedef struct
{
   CoolingMapKey_t coolingKey;
   FourDoorDualEvaporatorVotes_t votes;
} FourDoorDualEvaporatorStatesTable_t;

I_ConstArrayMap_t *ConstArrayMap_FourDoorDualEvap_Init(ConstArrayMap_FourDoorDualEvap_t *instance);

#endif

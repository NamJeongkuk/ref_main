/*!
 * @file
 * @brief Side by Side specific state based grid voting table.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONSTARRAYMAP_SIDEBYSIDESINGLEEVAP_H
#define CONSTARRAYMAP_SIDEBYSIDESINGLEEVAP_H

#include "CoolingMode.h"
#include "CoolingSpeed.h"
#include "FanSpeed.h"
#include "CompressorVotedSpeed.h"
#include "DamperVotedPosition.h"
#include "I_ConstArrayMap.h"
#include "CoolingMapKey.h"
#include "ConstArrayMap_LinearSearch.h"

typedef struct
{
   struct
   {
      ConstArrayMap_LinearSearch_t constArrayMap;
   } _private;
} ConstArrayMap_SideBySideSingleEvap_t;

typedef struct
{
   CompressorSpeed_t compressorSpeed;
   FanSpeed_t condenserFanSpeed;
   FanSpeed_t freezerEvapFanSpeed;
   DamperPosition_t freshFoodDamperPosition;
} SideBySideSingleEvaporatorVotes_t;

typedef struct
{
   CoolingMapKey_t coolingKey;
   SideBySideSingleEvaporatorVotes_t votes;
} SideBySideSingleEvaporatorStatesTable_t;

I_ConstArrayMap_t *ConstArrayMap_SideBySideSingleEvap_Init(ConstArrayMap_SideBySideSingleEvap_t *instance);

#endif

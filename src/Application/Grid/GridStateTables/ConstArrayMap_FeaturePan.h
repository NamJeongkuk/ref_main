/*!
 * @file
 * @brief Feature Pan Control specific state based grid voting table.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONSTARRAYMAP_FEATUREPAN_H
#define CONSTARRAYMAP_FEATUREPAN_H

#include "ConstArrayMap_LinearSearch.h"
#include "DamperVotedPosition.h"
#include "FeaturePanMode.h"
#include "FanSpeed.h"
#include "HeaterVotedState.h"
#include "I_ConstArrayMap.h"

typedef struct
{
   struct
   {
      ConstArrayMap_LinearSearch_t constArrayMap;
   } _private;
} ConstArrayMap_FeaturePan_t;

typedef struct
{
   DamperPosition_t damperPosition;
   HeaterState_t heaterState;
   FanSpeed_t fanSpeed;
} FeaturePanVotes_t;

typedef struct
{
   FeaturePanCoolingMode_t featurePanCoolingModeKey;
   FeaturePanVotes_t votes;
} FeaturePanStatesTable_t;

I_ConstArrayMap_t *ConstArrayMap_FeaturePan_Init(ConstArrayMap_FeaturePan_t *instance);

#endif

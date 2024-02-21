/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONSTARRAYMAP_SINGLEDOORSINGLEEVAP_H
#define CONSTARRAYMAP_SINGLEDOORSINGLEEVAP_H

#include "ConstArrayMap_LinearSearch.h"
#include "CoolingSpeed.h"
#include "CompressorVotedSpeed.h"
#include "FanSpeed.h"

typedef struct
{
   struct
   {
      ConstArrayMap_LinearSearch_t constArrayMap;
   } _private;
} ConstArrayMap_SingleDoorSingleEvap_t;

typedef struct
{
   CompressorSpeed_t compressorSpeed;
   FanSpeed_t condenserFanSpeed;
   FanSpeed_t primaryEvapFanSpeed;
} SingleDoorSingleEvaporatorVotes_t;

typedef struct
{
   CoolingSpeed_t speed;
   SingleDoorSingleEvaporatorVotes_t votes;
} SingleDoorSingleEvaporatorStatesTable_t;

I_ConstArrayMap_t *ConstArrayMap_SingleDoorSingleEvap_Init(ConstArrayMap_SingleDoorSingleEvap_t *instance);

#endif

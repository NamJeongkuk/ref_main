/*!
 * @file
 * @brief handles reporting all door states as closed while in sabbath or enhanced sabbath mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SABBATHINHIBITDOORS_H
#define SABBATHINHIBITDOORS_H

#include "I_DataModel.h"
#include "Erd.h"

typedef struct
{
   Erd_t doorStatusErd; // bool
   Erd_t doorStatusResolvedErd; // bool
} SabbathDoorResolvedPair_t;

typedef struct
{
   const SabbathDoorResolvedPair_t *doorResolvedPairErdList;
   uint8_t numberOfPairs;
   Erd_t sabbathModeErd; // bool
   Erd_t enhancedSabbathModeErd; // bool
   Erd_t sabbathGpioErd; // bool
} SabbathInhibitDoorsConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onDataModelChangeSubscription;
      const SabbathInhibitDoorsConfiguration_t *config;
   } _private;
} SabbathInhibitDoors_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void SabbathInhibitDoors_Init(
   SabbathInhibitDoors_t *instance,
   I_DataModel_t *dataModel,
   const SabbathInhibitDoorsConfiguration_t *config);

#endif

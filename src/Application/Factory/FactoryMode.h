/*!
 * @file
 * @brief operate Factory Mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FACTORYMODE_H
#define FACTORYMODE_H

#include "I_DataModel.h"
#include "ErdList.h"

typedef struct
{
   Erd_t factoryVoteErd;
   uint32_t offValue;
} FactoryVotePair_t;

typedef struct
{
   const FactoryVotePair_t *pairs;
   uint16_t numberOfPairs;
} FactoryVoteList_t;

typedef struct
{
   Erd_t factoryModeActiveErd; // bool
   Erd_t resetErd; // uint8_t
   const FactoryVoteList_t factoryVoteList;
} FactoryModeConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t factoryModeSubscription;
      const FactoryModeConfiguration_t *config;
   } _private;
} FactoryMode_t;

/*!
 * Initialize Factory Mode
 * @param instance
 * @param dataModel
 * @param factoryModeConfiguration
 */
void FactoryMode_Init(
   FactoryMode_t *instance,
   I_DataModel_t *dataModel,
   const FactoryModeConfiguration_t *factoryModeConfiguration);

#endif

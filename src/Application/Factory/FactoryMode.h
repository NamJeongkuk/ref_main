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
#include "Timer.h"

typedef struct
{
   Erd_t erd;
   uint32_t offValue;
} ErdOffValuePair_t;

typedef struct
{
   const ErdOffValuePair_t *pairs;
   uint16_t numberOfPairs;
} FactoryModeList_t;

typedef struct
{
   Erd_t factoryModeTimeErd; // uint8_t
   Erd_t broadcastResetRequestErd; // Signal_t
   const FactoryModeList_t factoryVoteList;
   const ErdList_t *lightVoteErdList;
   const FactoryModeList_t *erdValuePairList;
} FactoryModeConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t factoryModeSubscription;
      const FactoryModeConfiguration_t *config;
      Timer_t factoryModeOneMinuteTimer;
      TimerModule_t *timerModule;
      bool factoryModeEntered;
   } _private;
} FactoryMode_t;

/*!
 * Initialize Factory Mode
 * @param instance
 * @param dataModel
 * @param factoryModeConfiguration
 * @param timerModule
 */
void FactoryMode_Init(
   FactoryMode_t *instance,
   I_DataModel_t *dataModel,
   const FactoryModeConfiguration_t *factoryModeConfiguration,
   TimerModule_t *timerModule);
#endif

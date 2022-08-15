/*!
 * @file
 * @brief Counts the time that the compressor has been on for while the system
 * is actively waiting for the next defrost
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTCOMPRESSORONTIMECOUNTER_H
#define DEFROSTCOMPRESSORONTIMECOUNTER_H

#include "I_DataModel.h"
#include "Fsm.h"
#include "DefrostData.h"
#include "GridData.h"
#include "Timer.h"
#include "Constants_Binary.h"

typedef struct
{
   Erd_t compressorIsOnErd; // bool
   Erd_t activelyWaitingForNextDefrostErd; // bool
   Erd_t defrostCompressorOnTimeInSecondsErd; // uint32_t
   Erd_t defrostCompressorOnTimeCounterFsmStateErd; // DefrostCompressorOnTimeCounterFsmState_t
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpErd; // bool
   Erd_t activelyWaitingForDefrostOnCompareMatchReadyErd; // bool
   Erd_t freezerFilteredTemperatureTooWarmOnPowerUpReadyErd; // bool
   Erd_t defrostCompressorOnTimeCounterReadyErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DefrostCompressorOnTimeCounterConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      Timer_t periodicTimer;
      EventSubscription_t subscription;
      const DefrostCompressorOnTimeCounterConfiguration_t *config;
   } _private;
} DefrostCompressorOnTimeCounter_t;

void DefrostCompressorOnTimeCounter_Init(
   DefrostCompressorOnTimeCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostCompressorOnTimeCounterConfiguration_t *config);

#endif

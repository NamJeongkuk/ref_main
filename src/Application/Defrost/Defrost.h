/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROST_H
#define DEFROST_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "SabbathData.h"
#include "GridData.h"
#include "Timer.h"
#include "Hsm.h"

typedef struct
{
   Erd_t defrostHsmStateErd; // DefrostHsmState_t
   Erd_t defrostDoorHoldoffRequestErd; // bool
   Erd_t defrostTimerCounterRequestErd; // DefrostTimerCounterRequest_t
   Erd_t freezerFilteredTemperatureErd; // TemperatureDegFx100_t
   Erd_t calculatedGridLinesErd; // CalculatedGridLines_t
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t numberOfFzAbnormalDefrostCyclesErd; // uint16_t
   Erd_t fzDefrostWasAbnormalErd; // bool
   Erd_t fzAbnormalDefrostCycleCountErd; // uint16_t
   Erd_t fzDefrostCycleCountErd; // uint16_t
   Erd_t ffDefrostHeaterDefrostVoteErd; // HeaterVotedState_t
   Erd_t fzDefrostHeaterDefrostVoteErd; // HeaterVotedState_t
   Erd_t defrostTimerCounterFsmStateErd; // DefrostTimerCounterFsmState_t
   Erd_t sealedSystemValvePositionErd; // ValvePosition_t
   Erd_t defrostTimerIsSatisfiedErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DefrostConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      union
      {
         Timer_t powerUpDelayTimer;
         Timer_t timeBetweenDefrostsTimer;
      };
      Timer_t periodicTimer;
      Timer_t prechillHoldoffTimer;
      bool resetRequiredWhenEnablingDefrostTimerCounter;
      const DefrostConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      const GridData_t *gridParametricData;
   } _private;
} Defrost_t;

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig);

#endif

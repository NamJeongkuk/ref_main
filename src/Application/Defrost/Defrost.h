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
#include "EvaporatorData.h"
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
   Erd_t freezerEvaporatorThermistorIsValidErd; // bool
   Erd_t numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd; // uint8_t
   Erd_t iceCabinetFanDefrostVoteErd; // FanVotedSpeed_t
   Erd_t freezerSetpointDefrostVoteErd; // SetpointVotedTemperature_t
   Erd_t freezerResolvedSetpointErd; // SetpointVotedTemperature_t
   Erd_t sealedSystemValvePositionDefrostVoteErd; // ValveVotedPosition_t
   Erd_t defrostIsFreshFoodOnlyErd; // bool
   Erd_t freshFoodSetpointDefrostVoteErd; // SetpointVotedTemperature_t
   Erd_t extendDefrostSignalErd; // Signal_t
   Erd_t compressorStateErd; // CompressorState_t
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
      Timer_t maxPrechillHoldoffTimer;
      EventSubscription_t dataModelSubscription;
      const DefrostConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      const GridData_t *gridParametricData;
      const EvaporatorData_t *evaporatorParametricData;
      bool resetRequiredWhenEnablingDefrostTimerCounter;
   } _private;
} Defrost_t;

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig);

#endif

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
   Erd_t freezerFilteredTemperatureResolvedErd; // TemperatureDegFx100_t
   Erd_t calculatedGridLinesErd; // CalculatedGridLines_t
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t numberOfFreezerAbnormalDefrostCyclesErd; // uint16_t
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t freezerAbnormalDefrostCycleCountErd; // uint16_t
   Erd_t freshFoodDefrostWasAbnormalErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t freezerDefrostCycleCountErd; // uint16_t
   Erd_t freshFoodDefrostHeaterDefrostVoteErd; // HeaterVotedState_t
   Erd_t freezerDefrostHeaterDefrostVoteErd; // HeaterVotedState_t
   Erd_t sealedSystemValvePositionErd; // ValvePosition_t
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
   Erd_t defrostMaxHoldoffMetErd; // bool
   Erd_t defrostPrechillRunCounterInMinutesErd; // uint16_t
   Erd_t timeInMinutesInValvePositionBErd; // uint32_t
   Erd_t prechillTimeMetErd; // bool
   Erd_t maxPrechillTimeInMinutesErd; // uint8_t
   Erd_t freezerEvaporatorFilteredTemperatureErd; // TemperatureDegFx100_t
   Erd_t adjustedFreezerSetpointErd; // TemperatureDegFx100_t
   Erd_t doorHoldoffTimeIsSatisfiedErd; // bool
   Erd_t noFreezeLimitIsActiveErd; // bool
   Erd_t freezerEvapFanDefrostVoteErd; // FanVotedSpeed_t
   Erd_t freshFoodEvapFanDefrostVoteErd; // FanVotedSpeed_t
   Erd_t defrostReadyTimerIsSatisfied; // bool
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DefrostConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      Timer_t powerUpDelayTimer;
      Timer_t periodicTimer;
      EventSubscription_t dataModelSubscription;
      uint32_t prechillRunCounterInMinutes;
      const DefrostConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      const GridData_t *gridParametricData;
      const EvaporatorData_t *evaporatorParametricData;
   } _private;
} Defrost_t;

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig);

#endif

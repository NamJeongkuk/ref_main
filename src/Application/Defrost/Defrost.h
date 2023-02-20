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
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t waitingForDefrostErd; // bool
   Erd_t defrostingErd; // bool
   Erd_t disableDefrostErd; // bool
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t freshFoodDefrostWasAbnormalErd; // bool
   Erd_t hasConvertibleCompartment; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t readyToDefrost; // bool
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpErd; // bool
   Erd_t freezerEvaporatorThermistorIsValidErd; // bool
   Erd_t freshFoodThermistorIsValidErd; // bool
   Erd_t compressorIsOnErd; // bool
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t freezerFilteredTemperatureTooWarmOnPowerUpReadyErd; // bool
   Erd_t disableMinimumTimeRequestErd; // bool
   Erd_t maxPrechillTimeInMinutesErd; // uint8_t
   Erd_t timeThatPrechillConditionsAreMetInMinutesErd; // uint16_t
   Erd_t compressorSpeedVoteErd; // CompressorVotedSpeed_t
   Erd_t condenserFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t freezerEvapFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t iceCabinetFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t freshFoodDamperPositionVoteErd; // DamperVotedPosition_t
   Erd_t freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freezerFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freshFoodFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freezerDefrostHeaterVoteErd; // HeaterVotedState_t
   Erd_t freezerDefrostCycleCountErd; // uint16_t
   Erd_t numberOfFreezerAbnormalDefrostCycleCountErd; // uint16_t
   Erd_t freezerDefrostHeaterOnTimeInMinutesErd; // uint8_t
   Erd_t freezerDefrostHeaterMaxOnTimeInMinutesErd; // uint8_t
   Erd_t nextDefrostTypeErd; // DefrostType_t
   Erd_t currentDefrostTypeErd; // DefrostType_t
   Erd_t timerModuleErd; // TimerModule_t *
   Erd_t clearedEepromStartup; // bool
   Erd_t defrostTestStateRequestErd; // DefrostTestStateRequestMessage_t
   Erd_t dontSkipDefrostPrechillErd; // bool
   Erd_t invalidFreezerEvaporatorThermistorDuringDefrostErd; // bool
   Erd_t useMinimumReadyToDefrostTimeErd; // bool
} DefrostConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      Timer_t defrostTimer;
      EventSubscription_t dataModelSubscription;
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

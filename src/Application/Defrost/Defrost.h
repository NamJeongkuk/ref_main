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
   Erd_t hasConvertibleCompartmentErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t readyToDefrostErd; // bool
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpErd; // bool
   Erd_t freezerEvaporatorThermistorIsValidErd; // bool
   Erd_t freshFoodThermistorIsValidResolvedErd; // bool
   Erd_t freezerThermistorIsValidResolvedErd; // bool
   Erd_t compressorIsOnErd; // bool
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t freezerFilteredTemperatureTooWarmOnPowerUpReadyErd; // bool
   Erd_t disableCompressorMinimumTimesVoteErd; // BooleanVotedState_t
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
   Erd_t numberOfFreezerDefrostsErd; // uint16_t
   Erd_t numberOfFreezerAbnormalDefrostsErd; // uint16_t
   Erd_t freezerDefrostHeaterOnTimeInMinutesErd; // uint8_t
   Erd_t freezerDefrostHeaterMaxOnTimeInMinutesErd; // uint8_t
   Erd_t nextDefrostTypeOverrideErd; // DefrostType_t
   Erd_t currentDefrostTypeErd; // DefrostType_t
   Erd_t timerModuleErd; // TimerModule_t *
   Erd_t eepromClearedErd; // bool
   Erd_t defrostTestStateRequestErd; // DefrostTestStateRequestMessage_t
   Erd_t dontSkipDefrostPrechillErd; // bool
   Erd_t invalidFreezerEvaporatorThermistorDuringDefrostErd; // bool
   Erd_t useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd; // bool
   Erd_t sabbathModeErd; // bool
   Erd_t enhancedSabbathModeErd; // bool
   Erd_t sabbathIsReadyToDefrostErd; // bool
   Erd_t enhancedSabbathIsRequestingDefrostErd; // bool
   Erd_t freezerDefrostHeaterOnForMaxTimeFaultErd; // bool
   Erd_t freshFoodDefrostHeaterVoteErd; // HeaterVotedState_t
   Erd_t convertibleCompartmentDefrostHeaterVoteErd; // HeaterVotedState_t
   Erd_t freshFoodEvapFanVoteErd; // FanVotedSpeed_t
   Erd_t convertibleCompartmentEvapFanVoteErd; // FanVotedSpeed_t
   Erd_t sealedSystemValveHomingRequestErd; // bool
   Erd_t sealedSystemValvePositionVoteErd; // SealedSystemValveVotedPosition_t
   Erd_t sealedSystemValvePositionGridVoteErd; // SealedSystemValveVotedPosition_t
   Erd_t freshFoodDamperPositionGridVoteErd; // DamperVotedPosition_t
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t convertibleCompartmentTemperatureErd; // TemperatureDegFx100_t
   Erd_t convertibleCompartmentThermistorIsValidErd; // bool
   Erd_t convertibleCompartmentDamperPositionVoteErd; // DamperVotedPosition_t
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
   } _private;
} Defrost_t;

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig,
   const DefrostData_t *defrostData);

#endif

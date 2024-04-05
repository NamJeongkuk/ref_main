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
#include "PlatformData.h"
#include "EnhancedSabbathData.h"
#include "Timer.h"
#include "Hsm.h"

typedef struct
{
   Erd_t heaterVoteErd; // HeaterVotedState_t
   Erd_t heaterRelayErd; // bool
   Erd_t evaporatorFanVoteErd; // FanVotedSpeed_t
   Erd_t defrostWasAbnormalErd; // bool
   Erd_t numberOfAbnormalDefrostsErd; // uint16_t
   Erd_t numberOfDefrostsErd; // uint16_t
   Erd_t defrostHeaterOnTimeInMinutesErd; // uint8_t
   Erd_t defrostHeaterMaxOnTimeInMinutesErd; // uint8_t
   Erd_t defrostHeaterOnForMaxTimeFaultErd; // bool
   Erd_t evaporatorFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t evaporatorThermistorIsValidResolvedErd; // bool
   size_t offsetInParametricForEvaporatorHeaterTerminationTemperature;
} EvaporatorConfiguration_t;

typedef struct
{
   EvaporatorConfiguration_t freshFoodEvaporator;
   EvaporatorConfiguration_t freezerEvaporator;
   EvaporatorConfiguration_t convertibleCompartmentEvaporator;

   Erd_t defrostHsmStateErd; // DefrostHsmState_t
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t waitingForDefrostErd; // bool
   Erd_t defrostingErd; // bool
   Erd_t disableDefrostErd; // bool
   Erd_t readyToDefrostErd; // bool
   Erd_t compressorIsOnErd; // bool
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t hasConvertibleCompartmentErd; // bool
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t cabinetFilteredTemperatureTooWarmOnPowerUpErd; // bool
   Erd_t defrostPowerUpReadyErd; // bool
   Erd_t disableCompressorMinimumTimesVoteErd; // BooleanVotedState_t
   Erd_t timeThatPrechillConditionsAreMetInMinutesErd; // uint16_t
   Erd_t compressorSpeedVoteErd; // CompressorVotedSpeed_t
   Erd_t condenserFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t iceCabinetFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t deliFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t freshFoodDamperPositionVoteErd; // DamperVotedPosition_t
   Erd_t nextDefrostTypeOverrideErd; // DefrostType_t
   Erd_t numberOfSecondaryOnlyDefrostsErd; // uint8_t
   Erd_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd; // uint8_t
   Erd_t currentDefrostTypeErd; // DefrostType_t
   Erd_t eepromClearedErd; // bool
   Erd_t sealedSystemValveHomingRequestErd; // bool
   Erd_t sealedSystemValvePositionVoteErd; // SealedSystemValveVotedPosition_t
   Erd_t sealedSystemValvePositionGridVoteErd; // SealedSystemValveVotedPosition_t
   Erd_t freshFoodDamperPositionGridVoteErd; // DamperVotedPosition_t
   Erd_t convertibleCompartmentDamperPositionVoteErd; // DamperPosition_t
   Erd_t deliDamperPositionVoteErd; // DamperPosition_t
   Erd_t calculatedGridLinesErd; // TwoDimensionalCalculatedGridLines_t

   Erd_t defrostTestStateRequestErd; // DefrostTestStateRequestMessage_t
   Erd_t dontSkipDefrostPrechillErd; // bool
   Erd_t invalidEvaporatorThermistorDuringDefrostErd; // bool
   Erd_t useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd; // bool

   Erd_t sabbathModeErd; // bool
   Erd_t enhancedSabbathModeErd; // bool
   Erd_t sabbathIsReadyToDefrostErd; // bool
   Erd_t enhancedSabbathIsRequestingDefrostErd; // bool

   Erd_t convertibleCompartmentFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t convertibleCompartmentThermistorIsValidResolvedErd; // bool

   Erd_t freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freezerEvaporatorThermistorIsValidResolvedErd; // bool
   Erd_t freezerEvaporatorThermistorHasBeenDiscoveredErd; // bool

   Erd_t freshFoodEvaporatorThermistorHasBeenDiscoveredErd; // bool
   Erd_t convertibleCompartmentEvaporatorThermistorHasBeenDiscoveredErd; // bool

   Erd_t freezerFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freezerThermistorIsValidResolvedErd; // bool
   Erd_t freezerThermistorHasBeenDiscoveredErd; // bool

   Erd_t freshFoodFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freshFoodThermistorIsValidResolvedErd; // bool
   Erd_t freshFoodThermistorHasBeenDiscoveredErd; // bool
} DefrostConfiguration_t;

typedef struct
{
   const EvaporatorConfiguration_t *evaporator0;
   const EvaporatorConfiguration_t *evaporator1;
} EvaporatorConfigs_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      Timer_t defrostTimer;
      EventSubscription_t dataModelSubscription;
      EvaporatorConfigs_t primaryEvaporatorsConfigs;
      EvaporatorConfigs_t secondaryEvaporatorsConfigs;
      const DefrostConfiguration_t *config;
      const DefrostData_t *defrostData;
      const PlatformData_t *platformData;
      const EnhancedSabbathData_t *enhancedSabbathData;
      bool skipDefrostTypeDeterminationDueToTestRequest;
      bool skipDefrostTypeDeterminationDueToDefrostDisable;
   } _private;
} Defrost_t;

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig,
   const DefrostData_t *defrostData,
   const PlatformData_t *platformData,
   const EnhancedSabbathData_t *enhancedSabbathData);

#endif

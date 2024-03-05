/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostConfiguration.h"
#include "SystemErds.h"

const DefrostConfiguration_t defrostConfiguration = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostStateErd = Erd_DefrostState,
   .waitingForDefrostErd = Erd_WaitingToDefrost,
   .defrostingErd = Erd_Defrosting,
   .disableDefrostErd = Erd_DisableDefrost,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .freezerThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .disableCompressorMinimumTimesVoteErd = Erd_DisableMinimumCompressorTimes_DefrostVote,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_DefrostVote,
   .freezerEvapFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_DefrostVote,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .freshFoodFilteredTemperatureResolvedInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerDefrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .numberOfFreezerDefrostsErd = Erd_NumberOfFreezerDefrosts,
   .numberOfFreezerAbnormalDefrostsErd = Erd_NumberOfFreezerAbnormalDefrosts,
   .freezerDefrostHeaterOnTimeInMinutesErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .timerModuleErd = Erd_TimerModule,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill,
   .invalidFreezerEvaporatorThermistorDuringDefrostErd = Erd_InvalidFreezerEvaporatorThermistorDuringDefrost,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .enhancedSabbathIsRequestingDefrostErd = Erd_EnhancedSabbathIsRequestingDefrost,
   .freezerDefrostHeaterOnForMaxTimeFaultErd = Erd_FreezerDefrostHeaterOnForMaxTimeFault,
   .freshFoodDefrostHeaterVoteErd = Erd_FreshFoodDefrostHeater_DefrostVote,
   .convertibleCompartmentDefrostHeaterVoteErd = Erd_ConvertibleCompartmentDefrostHeater_DefrostVote,
   .freshFoodEvapFanVoteErd = Erd_FreshFoodEvapFanSpeed_DefrostVote,
   .convertibleCompartmentEvapFanVoteErd = Erd_ConvertibleCompartmentEvapFanSpeed_DefrostVote,
   .sealedSystemValveHomingRequestErd = Erd_SealedSystemValveHomingRequest,
   .sealedSystemValvePositionVoteErd = Erd_SealedSystemValvePosition_DefrostVote,
   .sealedSystemValvePositionGridVoteErd = Erd_SealedSystemValvePosition_GridVote,
   .freshFoodDamperPositionGridVoteErd = Erd_FreshFoodDamperPosition_GridVote,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .convertibleCompartmentTemperatureErd = Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,
   .convertibleCompartmentThermistorIsValidErd = Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,
   .convertibleCompartmentDamperPositionVoteErd = Erd_ConvertibleCompartmentDamperPosition_DefrostVote
};

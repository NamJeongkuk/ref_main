/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Defrost.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Vote.h"
#include "DefrostConfiguration.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Invalid = false,
   Valid = true
};

enum
{
   ThreeHundredMinutes = 300,
   TwoHundredFiftyFiveMinutes = 255,
   TenMinutes = 10,
   NineMinutes = 9,
   OneMinute = 1,
   ZeroMinutes = 0,
};

enum
{
   FreezerDefrostHeaterMaxOnTimeInMinutes = 45
};

enum
{
   CaresAbout = Vote_Care,
   DoesNotCareAbout = Vote_DontCare,
};

static const DefrostConfiguration_t defrostConfig = {
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
   .convertibleCompartmentFilteredTemperatureResolvedInDegFx100Erd = Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,
   .convertibleCompartmentThermistorIsValidResolvedErd = Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,
   .convertibleCompartmentDamperPositionVoteErd = Erd_ConvertibleCompartmentDamperPosition_DefrostVote,
   .freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
   .freezerEvaporatorThermistorIsValidResolvedErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freezerEvaporatorThermistorHasBeenDiscoveredErd = Erd_FreezerEvaporatorThermistorDiscovered,
   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .freezerThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .freezerThermistorHasBeenDiscoveredErd = Erd_FreezerThermistorDiscovered,
   .freshFoodFilteredTemperatureResolvedInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freshFoodThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .freshFoodThermistorHasBeenDiscoveredErd = Erd_FreshFoodThermistorDiscovered
};

static const DefrostIdleData_t idleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR
};

static const DefrostPrechillPrepData_t prechillPrepData = {
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 0,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet = 0,
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t prechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForSecondaryOnlyDefrostInMinutes = 20,
   .prechillSealedSystemValvePosition = SealedSystemValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvapFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvapFanSpeed = FanSpeed_Low,
   .prechillIceCabinetFanSpeed = FanSpeed_Medium,
   .prechillDeliFanSpeed = FanSpeed_High,
   .prechillConvertibleCompartmentDamperPosition = DamperPosition_Open,
   .prechillConvertibleCompartmentFanSpeed = FanSpeed_High,
   .prechillFreshFoodDamperPosition = DamperPosition_Closed,
   .prechillFreezerMinTempInDegFx100 = -600,
   .prechillFreshFoodMinTempInDegFx100 = 3200,
   .prechillFreshFoodMaxTempInDegFx100 = 4600,
   .prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600,
   .prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200,
   .prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000
};

static const DefrostHeaterOnEntryData_t heaterOnEntryData = {
   .defrostHeaterOnDelayAfterCompressorOffInSeconds = 2,
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open,
   .heaterOnEntrySealedSystemValvePosition = SealedSystemValvePosition_A
};

static const DefrostHeaterOnData_t heaterOnData = {
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .freezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentAsFreezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 35,
   .convertibleCompartmentAsFreshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2,
   .convertibleCompartmentAsFreezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25
};

static const DefrostDwellData_t dwellData = {
   .dwellTimeInMinutes = 7,
   .dwellFreshFoodDamperPosition = DamperPosition_Closed,
   .dwellSealedSystemValvePosition = SealedSystemValvePosition_B
};

static const DefrostPostDwellData_t postDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .secondaryOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellFreezerEvapExitTemperatureInDegFx100 = -1000,
   .secondaryOnlyPostDwellFreezerEvapExitTemperatureInDegFx100 = -1200,
   .postDwellSealedSystemValvePosition = SealedSystemValvePosition_B,
   .postDwellCompressorSpeed = CompressorSpeed_Low,
   .postDwellCondenserFanSpeed = FanSpeed_Low,
   .postDwellFreshFoodDamperPosition = DamperPosition_Closed
};

static const DefrostData_t defrostData = {
   .idleData = idleData,
   .prechillPrepData = prechillPrepData,
   .prechillData = prechillData,
   .heaterOnEntryData = heaterOnEntryData,
   .heaterOnData = heaterOnData,
   .dwellData = dwellData,
   .postDwellData = postDwellData
};

TEST_GROUP(Defrost_SingleEvapSideBySide)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 1;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void ShouldAssertOnInitialization()
   {
      CHECK_ASSERTION_FAILED(DefrostIsInitializedWithoutAssertErdsSet());
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &state);
   }

   void GivenFreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      FreezerFilteredTemperatureTooWarmOnPowerUpIs(state);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void LastFreshFoodDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void LastFreshFoodDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, &state);
   }

   void LastConvertibleCompartmentDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void LastConvertibleCompartmentDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void HasConvertibleCompartment()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &state);
   }

   void LastFreezerDefrostWasNormal()
   {
      bool state = false;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void LastFreezerDefrostWasAbnormal()
   {
      bool state = true;
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            WhenPrechillConditionsAreMet();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;

         case DefrostHsmState_HeaterOnEntry:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

            if(currentDefrostType == DefrostType_SecondaryOnly)
            {
               After(defrostData.prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes * MSEC_PER_MIN);
            }
            else
            {
               After(defrostData.prechillData.maxPrechillTimeInMinutes * MSEC_PER_MIN);
            }
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
            break;

         case DefrostHsmState_HeaterOn:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

            After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
            break;

         case DefrostHsmState_Dwell:
            Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
            And DefrostStateIs(DefrostState_Dwell);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
            break;

         case DefrostHsmState_PostDwell:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

            After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
            DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
            break;

         case DefrostHsmState_Disabled:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

            When DisableDefrostIs(true);
            DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
            break;

         default:
            break;
      }
   }

   void GivenDefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostIsInitializedAndStateIs(state);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void ClearReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, clear);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void WhenPrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_Freezer);
   }

   void PrechillConditionsAreNotMet()
   {
      CompressorIs(OFF);
      CoolingModeIs(CoolingMode_FreshFood);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void CompressorSpeedVoteShouldBe(CompressorSpeed_t expected)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreezerEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvapFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodEvapFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentEvapFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentEvapFanSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodDamperPositionVoteShouldBe(DamperPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodDamperPositionVoteShouldBeDontCare()
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void PrechillLoadVotesShouldBeDontCare()
   {
      CompressorVotedSpeed_t compressorVote;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &compressorVote);

      FanVotedSpeed_t fanVote;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &fanVote);

      DamperVotedPosition_t damperVote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &damperVote);

      CHECK_FALSE(compressorVote.care);
      CHECK_FALSE(fanVote.care);
      CHECK_FALSE(damperVote.care);
   }

   void FreshFoodDamperVoteShouldBeDontCare()
   {
      DamperVotedPosition_t damperVote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &damperVote);

      CHECK_FALSE(damperVote.care);
   }

   void FreezerDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void ConvertibleCompartmentHeaterVoteShouldBeDontCare()
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void CompressorSpeedVoteShouldBeDontCare()
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void DisableMinimumCompressorTimesShouldBe(bool expectedState, Vote_t expectedCare)
   {
      BooleanVotedState_t actualVote;
      DataModel_Read(dataModel, Erd_DisableMinimumCompressorTimes_DefrostVote, &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_EQUAL(expectedCare, actualVote.care);
   }

   void FanSpeedVotesShouldBeDontCare()
   {
      FanVotedSpeed_t actual;

      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actual);
      CHECK_FALSE(actual.care);

      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actual);
      CHECK_FALSE(actual.care);

      DataModel_Read(dataModel, Erd_CondenserFanSpeed_DefrostVote, &actual);
      CHECK_FALSE(actual.care);
   }

   void FreezerDefrostHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &vote);
   }

   void FreezerDefrostHeaterVoteShouldBe(HeaterState_t expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_TRUE(actual.care);
   }

   void ConvertibleCompartmentDefrostHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &vote);
   }

   void ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_t expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodDefrostHeaterIsVoted(HeaterState_t state)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &vote);
   }

   void IceCabinetFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void IceCabinetFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void NumberOfFreezerDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreezerDefrosts, &count);
   }

   void NumberOfFreezerDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreezerDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfFreezerAbnormalDefrostsIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreezerAbnormalDefrosts, &count);
   }

   void NumberOfFreezerAbnormalDefrostsShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreezerAbnormalDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerDefrostHeaterOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnTimeInMinutes, &timeInMinutes);
   }

   void FreezerDefrostHeaterMaxOnTimeInMinutesIs(uint8_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterMaxOnTimeInMinutes, &timeInMinutes);
   }

   void FreezerDefrostWasAbnormalFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostWasAbnormal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WaitingToDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_WaitingToDefrost, &state);
   }

   void WaitingToDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_WaitingToDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DefrostingIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Defrosting, &state);
   }

   void DefrostingShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Defrosting, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DisableDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DisableDefrost, &state);
   }

   void NextDefrostTypeOverrideIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_NextDefrostTypeOverride, &defrostType);
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }

   void CurrentDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ClearedEepromOnStartupIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void GivenClearedEepromOnStartupIs(bool state)
   {
      ClearedEepromOnStartupIs(state);
   }

   void ClearedEepromOnStartupShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void InvalidFreezerEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &state);
   }

   void InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_InvalidFreezerEvaporatorThermistorDuringDefrost, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DefrostTestIsRequested(DefrostTestStateRequest_t request)
   {
      DefrostTestStateRequestMessage_t requestMessage;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &requestMessage);
      requestMessage.request = request;
      requestMessage.requestId++;
      DataModel_Write(dataModel, Erd_DefrostTestStateRequest, &requestMessage);
   }

   void WhenDefrostTestIsRequested(DefrostTestStateRequest_t request)
   {
      DefrostTestIsRequested(request);
   }

   void DefrostTestStateRequestShouldBeNone(void)
   {
      DefrostTestStateRequest_t expectedRequest = DefrostTestStateRequest_None;
      DefrostTestStateRequestMessage_t actual;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &actual);

      CHECK_EQUAL(expectedRequest, actual.request);
   }

   void DontSkipPrechillErdIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DontSkipDefrostPrechill, &state);
   }

   void DontSkipPrechillPrepErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DontSkipDefrostPrechill, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, &state);
   }

   void UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsFlagShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DefrostIsInPrechillPrepBecauseDontSkipPrechillErdWasSet()
   {
      Given LastFreshFoodDefrostWasNormal();
      And LastFreezerDefrostWasNormal();
      And LastConvertibleCompartmentDefrostWasNormal();
      And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
      And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
      And FreezerEvaporatorThermistorValidityIs(Valid);
      And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
      And FreezerThermistorValidityIs(Valid);
      And DontSkipPrechillErdIs(SET);

      When ReadyToDefrost();
      DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   }

   void DefrostTransitionsFromPrechillPrepToPrechillToHeaterOnEntryToHeaterOnToDwellToPostDwellThenToIdle()
   {
      CurrentDefrostTypeShouldBe(DefrostType_Full);

      WhenPrechillConditionsAreMet();
      DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

      After(defrostData.prechillData.maxPrechillTimeInMinutes * MSEC_PER_MIN - 1);
      DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

      After(1);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

      ClearReadyToDefrost();
      PrechillConditionsAreNotMet();

      After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
      DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);

      When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

      After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

      After(defrostData.postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathModeEnable, &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void WhenEnhancedSabbathModeIs(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void WhenSabbathIsReadyToDefrost()
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathIsReadyToDefrost,
         set);
   }

   void GivenSabbathIsReadyToDefrost()
   {
      WhenSabbathIsReadyToDefrost();
   }

   void WhenEnhancedSabbathIsRequestingToDefrost()
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathIsRequestingDefrost, set);
   }

   void WhenEnhancedSabbathIsNotRequestingToDefrost()
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathIsRequestingDefrost, clear);
   }

   void GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState()
   {
      Given LastFreshFoodDefrostWasNormal();
      And LastFreezerDefrostWasNormal();
      And LastConvertibleCompartmentDefrostWasNormal();
      And FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
      And FreezerThermistorValidityIs(Valid);
      And FreezerEvaporatorThermistorValidityIs(Valid);
      And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   }

   void FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeaterOnForMaxTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenFreezerDefrostHeaterOnForMaxTimeFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterOnForMaxTimeFault, &state);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void GivenFreshFoodThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistorDiscovered, set);
   }

   void FreshFoodDefrostHeaterVoteShouldBe(HeaterState_t expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_TRUE(actual.care);
   }

   void SealedSystemValveHomingRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SealedSystemValveHomingRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void SealedSystemValvePositionVoteShouldBe(SealedSystemValvePosition_t expected)
   {
      SealedSystemValveVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_SealedSystemValvePosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void SealedSystemValvePositionVoteShouldBeDontCare()
   {
      SealedSystemValveVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_SealedSystemValvePosition_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void GivenFreezerThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, clear);
   }

   void GivenFreezerEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, clear);
   }

   void ConvertibleCompartmentDamperVoteShouldBe(DamperPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDamperPosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void ConvertibleCompartmentDamperVoteShouldBeDontCare()
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDamperPosition_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void WhenFreshFoodDamperPositionGridVote(Vote_t care, DamperPosition_t position)
   {
      DamperVotedPosition_t vote = { position, care };
      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_GridVote, &vote);
   }

   void WhenSealedSystemValvePositionGridVote(Vote_t care, SealedSystemValvePosition_t position)
   {
      SealedSystemValveVotedPosition_t vote = { position, care };
      DataModel_Write(dataModel, Erd_SealedSystemValvePosition_GridVote, &vote);
   }

   void GivenConvertibleCompartmentCabinetThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved, &state);
   }

   void WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void GivenConvertibleCompartmentCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenConvertibleCompartmentCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenConvertibleCompartmentCabinetTemperatureIs(temperature);
   }

   void GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions()
   {
      Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
      Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
      Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
      GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
      Given FreezerEvaporatorThermistorValidityIs(Valid);
      Given FreezerThermistorValidityIs(Valid);
      Given FreshFoodThermistorValidityIs(Valid);
      GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
      Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   }
};

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPostDwell)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_PostDwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastFreshFoodDefrostWasAbnormal)
{
   Given LastFreshFoodDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastFreezerDefrostWasAbnormal)
{
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastConvertibleCompartmentDefrostWasAbnormal)
{
   Given HasConvertibleCompartment();
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerWasTooWarmAtPowerUpAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerWasTooWarmAtPowerUpAndLastFreezerDefrostWasAbnormalAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And LastFreezerDefrostWasAbnormal();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndInvalidFreezerEvaporatorThermistorDuringDefrostIsFalseAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(false);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasNotBeenDiscoveredAndInvalidFreezerEvaporatorThermistorDuringDefrostIsFalseAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(false);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenFreezerEvapThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsValidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Valid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreezerThermistorValidityIs(Valid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetDisableMinimumCompressorOnTimesWhenEnteringHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotSetDisableMinimumCompressorOnTimesWhenEnteringIdle)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotSetDisableMinimumCompressorOnTimesWhenEnteringPrechillPrep)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotSetDisableMinimumCompressorOnTimesWhenEnteringPrechill)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenReadyToDefrostAndLastDefrostsWereNormalAndThermistorsAreValidAndDontSkipPrechillPrepIsNotSet)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenReadyToDefrostAndInvalidFreezerEvapThermistorDuringDefrostIsTrueAndDontSkipPrechillIsSet)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   And DontSkipPrechillPrepErdShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldOnlyGoToPrechillPrepOnceWhenDontSkipPrechillIsSetIfConditionsAreNotPresentForItToGoToPrechillPrepNormallyAndInThisCaseItShouldGoToHeaterOnEntryInsteadOfPrechillPrepNextTime)
{
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInPrechillPrepBecauseDontSkipPrechillErdWasSet();

   When DefrostTransitionsFromPrechillPrepToPrechillToHeaterOnEntryToHeaterOnToDwellToPostDwellThenToIdle();
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisobeyTheDontSkipPrechillErdWhenFreezerThermistorIsInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisobeyTheDontSkipPrechillErdWhenFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And FreezerThermistorValidityIs(Valid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearUseAhamPrechillTimeErdOnEntryToPrechillPrep)
{
   Given UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsFlagIs(SET);
   And LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsFlagShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenPrechillConditionsAreMet();
   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToPrechillWhenPrechillConditionsNotMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIs(OFF);
   And CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   When CoolingModeIs(CoolingMode_FreshFood);
   And CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillWhenPrechillConditionsMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);

   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillWhenPrechillPrepTimerExpiresWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   After((defrostData.prechillPrepData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepTheNextTimeThroughIdleAfterItWasTooWarmOnPowerUp)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ClearReadyToDefrost();

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldExitPostDwellAndGoToIdleAfterFreshFoodOnlyPostDwellExitTimeIfPostDwellExitTimeIsFreshFoodOnly)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.secondaryOnlyPostDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldExitPostDwellAndGoToIdleAfterPostDwellExitTimeIfCurrentDefrostIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreezerThermistorIsAlreadyInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenEnteringPrechillPrepAndFreezerThermistorIsInvalidAndHasNotBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreezerThermistorIsAlreadyInvalidAndHasBeenDiscoveredWhileSabbathModeIsEnabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreezerThermistorIsAlreadyInvalidAndHasBeenDiscoveredWhileEnhancedSabbathModeIsEnabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenEnhancedSabbathModeIs(ENABLED);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenSabbathModeIsDisabledWhileReadyToDefrostAndFreezerThermistorIsInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenSabbathModeIsDisabledWhileReadyToDefrostAndFreezerThermistorIsInvalidAndHasNotBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   WhenSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnhancedSabbathModeIsDisabledWhileReadyToDefrostAndFreezerThermistorIsInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given ReadyToDefrost();

   WhenEnhancedSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenSabbathIsReadyToDefrostWhileSabbathModeIsEnabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);

   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileEnhancedSabbathModeIsEnabledAndSabbathModeIsDisabledIfEnhancedSabbathHasNotRequestedADefrost)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(DISABLED);

   WhenEnhancedSabbathIsNotRequestingToDefrost();
   WhenEnhancedSabbathModeIs(ENABLED);
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileEnhancedSabbathModeIsEnabledAndSabbathModeIsEnabledIfEnhancedSabbathHasNotRequestedADefrost)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsNotRequestingToDefrost();
   WhenEnhancedSabbathModeIs(ENABLED);
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileSabbathModeIsDisabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(DISABLED);

   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileEnhancedSabbathModeIsDisabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenEnhancedSabbathModeIs(DISABLED);

   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenSabbathModeIsEnabledWhileSabbathIsReadyToDefrost)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathIsReadyToDefrost();

   WhenSabbathModeIs(ENABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldStayInIdleWhenEnhancedSabbathModeIsEnabledWhileSabbathIsReadyToDefrostIfEnhancedSabbathHasNotRequestedADefrost)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Valid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   WhenEnhancedSabbathIsNotRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();

   WhenEnhancedSabbathModeIs(ENABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenFreezerThermistorBecomesInvalidWhileInPrechillPrepAndFreezerThermistorHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenFreezerThermistorBecomesInvalidWhileInPrechillPrepAndFreezerThermistorHasNotBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrepAndFreezerEvapThermistorHasNotBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechillForSecondaryOnlyDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);

   After(defrostData.prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechillForFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   After(defrostData.prechillData.maxPrechillTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeMinusTimeThatPrechillConditionsAreMetWhileTimeThatPrechillConditionsAreMetIsNotZeroInPrechillForSecondaryOnlyDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes - OneMinute);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);

   After(OneMinute * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeMinusTimeThatPrechillConditionsAreMetWhileTimeThatPrechillConditionsAreMetIsNotZeroInPrechillForFullDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeInMinutes - OneMinute);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();
   CurrentDefrostTypeShouldBe(DefrostType_Full);

   After(OneMinute * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndMaxPrechillTimeAlreadyMet)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndPrechillConditionsHaveBeenMetForLongerThanMaxPrechillTime)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeForSecondaryOnlyDefrostInMinutes + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerEvapTemperatureGoesBelowExitThresholdAndFreezerEvapThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 2);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerEvapTemperatureAlreadyBelowExitThresholdAndFreezerEvapThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreshFoodThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerTemperatureGoesBelowExitThresholdAndFreezerThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 2);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenInPrechillAndFreezerTemperatureGoesBelowExitThresholdIfFreezerThermistorIsInvalidAndFreezerThermistorHasNotBeenDiscovered)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreshFoodThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerTemperatureAlreadyBelowExitThreshold)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100);
   Given FreezerThermistorValidityIs(Valid);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreshFoodThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureDecreasesToExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureDecreasesToExitThresholdAndFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyAtExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillAndStayWhenPrechillConditionsAreMetDuringPrechillPrepWithFreshFoodTemperatureAlreadyAtExitThresholdAndFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillAndStayWhenPrechillConditionsAreMetDuringPrechillPrepWithFreezerTemperatureAlreadyAtExitThresholdAndFreezerThermistorIsInvalidAndHasNotBeenDiscovered)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillAndStayWhenPrechillConditionsAreMetDuringPrechillPrepWithFreezerEvapTemperatureAlreadyAtExitThresholdAndFreezerEvapThermistorIsInvalidAndHasNotBeenDiscovered)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureIncreasesToExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100 - 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureIncreasesToExitThresholdAndFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100 - 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenPrechillConditionsAreMetDuringPrechillPrepWithFreshFoodTemperatureAlreadyAtExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenFreezerThermistorBecomesInvalidWhileInPrechillAndFreezerThermistorHasBeenDiscovered)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenFreezerThermistorHasBeenDiscovered();
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When FreezerThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillAndFreezerEvapThermistorHasBeenDiscovered)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteForLoadsOnEntryToPrechill)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   CompressorSpeedVoteShouldBe(defrostData.prechillData.prechillCompressorSpeed);
   FreezerEvapFanSpeedVoteShouldBe(defrostData.prechillData.prechillFreezerEvapFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData.prechillData.prechillFreshFoodDamperPosition);
   IceCabinetFanSpeedVoteShouldBe(defrostData.prechillData.prechillIceCabinetFanSpeed);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTurnOffCompressorAndFansOnHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteForFreshFoodDamperBasedOnParametricallyDefinedPositionOnEntryToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   FreshFoodDamperPositionVoteShouldBe(DamperPosition_Open);
}

TEST(Defrost_SingleEvapSideBySide, ShouldReleaseControlOfHeaterOnEntryLoadsWhenDefrostIsDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   CompressorSpeedVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
   ConvertibleCompartmentDamperVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvapSideBySide, ShouldExitOnHeaterEntryStateAndTurnOnTheDefrostHeaterAndEnableMinimumCompressorTimesAfterDefrostHeaterOnDelayTimerExpired)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteCompressorAndFansOffAndFreshFoodDamperPositionBasedOnParametricallyDefinedPositionUponEntryToHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodDamperPositionVoteShouldBe(defrostData.heaterOnEntryData.heaterOnEntryFreshFoodDamperPosition);
   ConvertibleCompartmentDamperVoteShouldBe(DamperPosition_Closed);
   SealedSystemValvePositionVoteShouldBe(defrostData.heaterOnEntryData.heaterOnEntrySealedSystemValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldReleaseControlOfHeaterOnLoadsWhenDefrostIsDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   CompressorSpeedVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
   ConvertibleCompartmentDamperVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();

   When DisableDefrostIs(false);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperatureAndFreezerEvaporatorThermistorIsValid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   And NumberOfFreezerDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotTurnOffDefrostHeaterAndShouldNotIncrementFreezerDefrostCountAndShouldNotTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperatureAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);
   And NumberOfFreezerDefrostsShouldBe(1);
   And DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearFreezerDefrostWasAbnormalAndFreezerDefrostHeaterOnForMaxTimeFaultWhenFreezerHeaterOnTimeIsLessThanFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsValid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenFreezerDefrostHeaterOnForMaxTimeFaultIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsEqualToFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitionToDwellStateWhenFreezerHeaterOnTimeReachesMaxOnTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   And NumberOfFreezerDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountAndSetFreezerDefrostWasAbnormalAndSaveTheFreezerDefrostCountAsTheLastAbnormalFreezerDefrostCountWhenFreezerHeaterOnTimeReachesAbnormalTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostsIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given LastFreezerDefrostWasNormal();
   Given NumberOfFreezerDefrostsIs(3);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostsShouldBe(2);
   And FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountJustOnce)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostsIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostsShouldBe(2);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfFreezerAbnormalDefrostsShouldBe(2);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisableMinimumCompressorTimesAndRequestValveHomingAndVoteForDwellLoadsWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   SealedSystemValveHomingRequestShouldBe(SET);

   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   SealedSystemValvePositionVoteShouldBe(defrostData.dwellData.dwellSealedSystemValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteForFreshFoodDamperWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   FreshFoodDamperPositionVoteShouldBe(defrostData.dwellData.dwellFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPostDwellAfterDwellTimeHasPassed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisableMinimumCompressorTimesAndVoteForPostDwellLoadsWhenEnteringPostDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);

   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   CompressorSpeedVoteShouldBe(defrostData.postDwellData.postDwellCompressorSpeed);
   CondenserFanSpeedVoteShouldBe(defrostData.postDwellData.postDwellCondenserFanSpeed);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodDamperPositionVoteShouldBe(defrostData.postDwellData.postDwellFreshFoodDamperPosition);
   SealedSystemValvePositionVoteShouldBe(defrostData.postDwellData.postDwellSealedSystemValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAfterPostDwellTimeHasPassedForSecondaryOnlyDefrost)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.secondaryOnlyPostDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAfterPostDwellTimeHasPassedForFullDefrost)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperatureAndFreezerEvaporatorThermistorIsValid)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperatureAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperatureAndFreezerEvaporatorThermistorIsValid)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperatureAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotCareAboutPostDwellLoadsAndEnableMinimumCompressorTimesWhenEnteringIdleFromPostDwell)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);
   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   FreezerDefrostHeaterVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   FreshFoodEvapFanSpeedVoteShouldBeDontCare();
   ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringIdleOnInit)
{
   Given WaitingToDefrostIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WaitingToDefrostShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringPostDwellOnInit)
{
   Given WaitingToDefrostIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   WaitingToDefrostShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringPostDwellFromDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   WaitingToDefrostShouldBe(false);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   WaitingToDefrostIs(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteFalseToDefrostingErdWhenEnteringIdleOnInit)
{
   Given DefrostingIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteFalseToDefrostingErdWhenEnteringPostDwellOnInit)
{
   Given DefrostingIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteTrueToDefrostingErdWhenEnteringDwellOnInit)
{
   Given DefrostingIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   DefrostingShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteTrueToDefrostingErdWhenEnteringHeaterOnEntryOnInit)
{
   Given DefrostingIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   DefrostingShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteFalseToDefrostingErdWhenEnteringPostDwellFromDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   DefrostingShouldBe(true);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteFalseToWaitingToDefrostErdWhenEnteringPrechillPrepFromIdle)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   WaitingToDefrostShouldBe(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   WaitingToDefrostShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionFromWaitingToDefrostStateToDisabledWhenDisableIsRequested)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionFromDefrostingStateToDisabledWhenDisableIsRequested)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteFalseToWaitingToDefrostErdWhenEnteringDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   WaitingToDefrostShouldBe(true);

   When DisableDefrostIs(true);
   WaitingToDefrostShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteFalseToDefrostingErdWhenEnteringDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   DefrostingShouldBe(true);

   When DisableDefrostIs(true);
   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromHeaterOnToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromIdleToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);
   Given FreshFoodDefrostHeaterIsVoted(HeaterState_On);
   Given ConvertibleCompartmentDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off);
}

TEST(Defrost_SingleEvapSideBySide, ShouldReleaseControlOfFreezerDefrostHeaterWhenDefrostBecomesEnabledAndTransitionsFromDisabledToIdle)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);

   When DisableDefrostIs(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   FreezerDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvapSideBySide, ShouldWriteToCurrentDefrostTypeWhenExitingWaitingToDefrost)
{
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   And CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   And LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryFromIdleWhenReadyToDefrostAndEepromIsClearedOnStartup)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given ClearedEepromOnStartupIs(true);

   When ReadyToDefrost();
   ClearedEepromOnStartupShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetInvalidFreezerEvaporatorThermistorDuringDefrostAndNotSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(SET);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearInvalidFreezerEvaporatorThermistorDuringDefrostAndSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearInvalidFreezerEvaporatorThermistorDuringDefrostAndFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);
   GivenFreezerDefrostHeaterOnForMaxTimeFaultIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotClearInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsInvalidAndHasBeenDiscoveredAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInIdleState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleState)
{
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleStateButTheFreezerThermistorIsInvalidAndHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleStateButTheFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInIdleState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPostDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPostDwellState)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPostDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInHeaterOnState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInHeaterOnState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInHeaterOnState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestAndEnableMinimumCompressorTimesAndVoteDontCareForDwellLoadsWhenIdleTestIsRequestedInDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentHeaterVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBeDontCare();
   CondenserFanSpeedVoteShouldBeDontCare();
   FreezerEvapFanSpeedVoteShouldBeDontCare();
   FreshFoodEvapFanSpeedVoteShouldBeDontCare();
   ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare();
   IceCabinetFanSpeedVoteShouldBeDontCare();
   FreshFoodDamperPositionVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();

   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestAndEnableMinimumCompressorTimesWhenPrechillTestIsRequestedInDwellState)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldSkipPrechillPrepAndTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestAndNotEnableMinimumCompressorTimesWhenPrechillTestIsRequestedInDwellStateBecauseFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldSkipPrechillPrepAndTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInDwellStateBecauseFreezerThermistorIsInvalidAndHasBeenDiscovered)
{
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToDefrostDwellWhenExitDefrostHeaterOnStateIsRequestedInHeaterOnState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToDefrostDwellWhenExitDefrostHeaterOnStateIsRequestedInHeaterOnEntryState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInIdle)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInPrechillPrep)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInPrechill)
{
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInDwell)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInPostDwell)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenIdleTestIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenDefrostTestIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldDoNothingWhenPrechillTestIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvapSideBySide, ShouldThrowAssertionWhenFreezerFilteredTemperatureTooWarmOnPowerUpReadyIsSet)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);

   ShouldAssertOnInitialization();
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenRunningAPrechillTestAsFirstFullDefrost)
{
   Given FreezerThermistorValidityIs(true);
   Given FreezerEvaporatorThermistorValidityIs(true);
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenRunningAnIdleTestAfterItExitsIdle)
{
   Given FreezerThermistorValidityIs(true);
   Given FreezerEvaporatorThermistorValidityIs(true);
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle); // when requesting Idle test while in Idle, it exits and reenters Idle
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenRunningADefrostTestAsFirstFullDefrost)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenDontSkipPrechillPrepFlagIsSet)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenSabbathIsReadyToDefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);

   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionFromIdleToHeaterOnEntryWhenSabbathIsReadyToDefrostAndBothSabbathAndEnhancedSabbathModesAreActiveAndEnhancedSabbathIsRequestingADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionFromIdleToHeaterOnEntryWhenSabbathIsReadyToDefrostAndSabbathModeIsInactiveAndEnhancedSabbathModeIsActiveAndEnhancedSabbathIsRequestingADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionFromIdleToPrechillPrepWhenSabbathIsReadyToDefrostAndSabbathModeIsActiveAndEnhancedSabbathModeIsInactiveAndEnhancedSabbathIsRequestingADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(DISABLED);

   WhenEnhancedSabbathIsRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldStayInIdleWhenSabbathIsReadyToDefrostAndBothSabbathAndEnhancedSabbathAreActiveAndEnhancedSabbathModeDidNotRequestADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsNotRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToHeaterOnEntryWhenEnhancedSabbathIsRequestingADefrostAndSabbathIsAlreadyReadyToDefrostAndEnhancedSabbathIsAlreadyActive)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenSabbathIsReadyToDefrost();
   WhenEnhancedSabbathIsRequestingToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndFreezerFilteredTemperatureIsTooWarmOnPowerUpWhileDefrostStateIsHeaterOnOnInit)
{
   GivenFreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   GivenFreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_Prechill);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   GivenFreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   GivenFreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_Dwell);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldPreserveDamperPositionGridVoteWhenPrechillConditionsAreMetIfGridCares)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   DamperPosition_t someOtherDamperPosition = defrostData.prechillData.prechillFreshFoodDamperPosition + 1;
   WhenFreshFoodDamperPositionGridVote(CaresAbout, someOtherDamperPosition);
   WhenPrechillConditionsAreMet();
   FreshFoodDamperPositionVoteShouldBe(someOtherDamperPosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldPreserveSealedSystemValvePositionGridVoteWhenPrechillConditionsAreMetIfGridCares)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   SealedSystemValvePosition_t someOtherValvePosition = defrostData.prechillData.prechillSealedSystemValvePosition + 1;
   WhenSealedSystemValvePositionGridVote(CaresAbout, someOtherValvePosition);
   WhenPrechillConditionsAreMet();
   SealedSystemValvePositionVoteShouldBe(someOtherValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotPreserveDamperPositionGridVoteWhenPrechillConditionsAreMetIfGridDoesNotCare)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   DamperPosition_t someOtherDamperPosition = defrostData.prechillData.prechillFreshFoodDamperPosition + 1;
   WhenFreshFoodDamperPositionGridVote(DoesNotCareAbout, someOtherDamperPosition);
   WhenPrechillConditionsAreMet();
   FreshFoodDamperPositionVoteShouldBe(defrostData.prechillData.prechillFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotPreserveSealedSystemValvePositionGridVoteWhenPrechillConditionsAreMetIfGridDoesNotCare)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   SealedSystemValvePosition_t someOtherValvePosition = defrostData.prechillData.prechillSealedSystemValvePosition + 1;
   WhenSealedSystemValvePositionGridVote(DoesNotCareAbout, someOtherValvePosition);
   WhenPrechillConditionsAreMet();
   SealedSystemValvePositionVoteShouldBe(defrostData.prechillData.prechillSealedSystemValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndConvertibleCompartmentCabinetAsFreezerTemperatureIsMinAndThermistorIsValid)
{
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 + 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndConvertibleCompartmentCabinetAsFreshFoodTemperatureIsMinAndThermistorIsValid)
{
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenInPrechillAndConvertibleCompartmentCabinetAsFreshFoodTemperatureIsMaxAndThermistorIsValid)
{
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenInPrechillAndConvertibleCompartmentCabinetExitConditionIsHitButThermistorIsNotValid)
{
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given FreshFoodThermistorValidityIs(Valid);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreezerMinTempInDegFx100);
   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenPrechillIsEnteredIfConvertibleCompartmentCabinetAsFreezerTemperatureIsMinAndThermistorIsValid)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenPrechillIsEnteredIfConvertibleCompartmentCabinetAsFreshFoodTemperatureIsMinAndThermistorIsValid)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenPrechillIsEnteredIfConvertibleCompartmentCabinetAsFreshFoodTemperatureIsMaxAndThermistorIsValid)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);

   WhenConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   WhenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST_GROUP(Defrost_SingleEvapAllFreezer)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleDoorSingleEvapAllFreezer);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 1;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

      switch(state)
      {
         case DefrostHsmState_Idle:
            GivenDefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreezerThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         default:
            break;
      }
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }
};

TEST(Defrost_SingleEvapAllFreezer, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToPrechillWhenEnteringPrechillPrepAndCompressorIsOnAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapAllFreezer, ShouldGoToPrechillWhenCompressorTurnsOnWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST_GROUP(Defrost_SingleEvapAllFreshFood)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleDoorSingleEvapAllFreshFood);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      platformData.numberOfEvaporators = 1;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

      switch(state)
      {
         case DefrostHsmState_Idle:
            GivenDefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreezerThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         default:
            break;
      }
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t convertibleCompartmentStateType)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &convertibleCompartmentStateType);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTripleEvapPrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_ConvertibleCompartment);
   }

   void GivenFreezerThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, clear);
   }

   void GivenFreezerEvapThermistorHasNotBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, clear);
   }
};

TEST(Defrost_SingleEvapAllFreshFood, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapAllFreshFood, ShouldGoToPrechillWhenEnteringPrechillPrepAndCompressorIsOnAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapAllFreshFood, ShouldGoToPrechillWhenCompressorTurnsOnWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvapAllFreshFood, ShouldGoToPrechillWhenEnteringPrechillPrepIfFreezerTemperatureIsLessThanPrechillMinFreezerTemperatureIfFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasNotBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

static const DefrostIdleData_t dualEvapIdleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR
};

static const DefrostPrechillPrepData_t dualEvapPrechillPrepData = {
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 2,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet = 1,
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t dualEvapPrechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForSecondaryOnlyDefrostInMinutes = 20,
   .prechillSealedSystemValvePosition = SealedSystemValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvapFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvapFanSpeed = FanSpeed_Low,
   .prechillIceCabinetFanSpeed = FanSpeed_Medium,
   .prechillDeliFanSpeed = FanSpeed_High,
   .prechillConvertibleCompartmentDamperPosition = DamperPosition_Open,
   .prechillConvertibleCompartmentFanSpeed = FanSpeed_High,
   .prechillFreshFoodDamperPosition = DamperPosition_Closed,
   .prechillFreezerMinTempInDegFx100 = -600,
   .prechillFreshFoodMinTempInDegFx100 = 3200,
   .prechillFreshFoodMaxTempInDegFx100 = 4600,
   .prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600,
   .prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200,
   .prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000
};

static const DefrostHeaterOnEntryData_t dualEvapHeaterOnEntryData = {
   .defrostHeaterOnDelayAfterCompressorOffInSeconds = 2,
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open,
   .heaterOnEntrySealedSystemValvePosition = SealedSystemValvePosition_A
};

static const DefrostHeaterOnData_t dualEvapHeaterOnData = {
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .freezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentAsFreezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 35,
   .convertibleCompartmentAsFreshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2,
   .convertibleCompartmentAsFreezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25
};

static const DefrostDwellData_t dualEvapDwellData = {
   .dwellTimeInMinutes = 7,
   .dwellFreshFoodDamperPosition = DamperPosition_Closed,
   .dwellSealedSystemValvePosition = SealedSystemValvePosition_B
};

static const DefrostPostDwellData_t dualEvapPostDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .secondaryOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellFreezerEvapExitTemperatureInDegFx100 = -1000,
   .secondaryOnlyPostDwellFreezerEvapExitTemperatureInDegFx100 = -1200,
   .postDwellSealedSystemValvePosition = SealedSystemValvePosition_B,
   .postDwellCompressorSpeed = CompressorSpeed_Low,
   .postDwellCondenserFanSpeed = FanSpeed_Low,
   .postDwellFreshFoodDamperPosition = DamperPosition_Closed
};

static const DefrostData_t dualEvapDefrostData = {
   .idleData = dualEvapIdleData,
   .prechillPrepData = dualEvapPrechillPrepData,
   .prechillData = dualEvapPrechillData,
   .heaterOnEntryData = dualEvapHeaterOnEntryData,
   .heaterOnData = dualEvapHeaterOnData,
   .dwellData = dualEvapDwellData,
   .postDwellData = dualEvapPostDwellData
};

TEST_GROUP(Defrost_DualEvapThreeDoor)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapThreeDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 2;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &dualEvapDefrostData, &platformData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

      switch(state)
      {
         case DefrostHsmState_Idle:
            GivenDefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreezerThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         default:
            break;
      }
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t convertibleCompartmentStateType)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &convertibleCompartmentStateType);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTripleEvapPrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_ConvertibleCompartment);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }
};

TEST(Defrost_DualEvapThreeDoor, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsFreshFoodAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapThreeDoor, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValid)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapThreeDoor, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapThreeDoor, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapThreeDoor, ShouldGoToPrechillWhenCoolingModeChangesToFreezerAndPrechillConditionsMetWhileInPrechillPrep)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST_GROUP(Defrost_DualEvapFourDoor)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 2;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &dualEvapDefrostData, &platformData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

      switch(state)
      {
         case DefrostHsmState_Idle:
            GivenDefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreezerThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         default:
            break;
      }
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t convertibleCompartmentStateType)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &convertibleCompartmentStateType);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTripleEvapPrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_ConvertibleCompartment);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }
};

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsFreshFoodAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValid)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_DualEvapFourDoor, ShouldGoToPrechillWhenCoolingModeChangesToFreezerAndPrechillConditionsMetWhileInPrechillPrep)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_FreshFood);

   When CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

static const DefrostIdleData_t tripleEvapIdleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR
};

static const DefrostPrechillPrepData_t tripleEvapPrechillPrepData = {
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 2,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet = 1,
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t tripleEvapPrechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForSecondaryOnlyDefrostInMinutes = 20,
   .prechillSealedSystemValvePosition = SealedSystemValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvapFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvapFanSpeed = FanSpeed_Low,
   .prechillIceCabinetFanSpeed = FanSpeed_Medium,
   .prechillDeliFanSpeed = FanSpeed_High,
   .prechillConvertibleCompartmentDamperPosition = DamperPosition_Open,
   .prechillConvertibleCompartmentFanSpeed = FanSpeed_High,
   .prechillFreshFoodDamperPosition = DamperPosition_Closed,
   .prechillFreezerMinTempInDegFx100 = -600,
   .prechillFreshFoodMinTempInDegFx100 = 3200,
   .prechillFreshFoodMaxTempInDegFx100 = 4600,
   .prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600,
   .prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200,
   .prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000
};

static const DefrostHeaterOnEntryData_t tripleEvapHeaterOnEntryData = {
   .defrostHeaterOnDelayAfterCompressorOffInSeconds = 2,
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open,
   .heaterOnEntrySealedSystemValvePosition = SealedSystemValvePosition_A
};

static const DefrostHeaterOnData_t tripleEvapHeaterOnData = {
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .freezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodHeaterOnTimeToSetAbnormalDefrostInMinutes = 21,
   .convertibleCompartmentAsFreezerHeaterOnTimeToSetAbnormalDefrostInMinutes = 35,
   .convertibleCompartmentAsFreshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2,
   .convertibleCompartmentAsFreezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25
};

static const DefrostDwellData_t tripleEvapDwellData = {
   .dwellTimeInMinutes = 7,
   .dwellFreshFoodDamperPosition = DamperPosition_Closed,
   .dwellSealedSystemValvePosition = SealedSystemValvePosition_B
};

static const DefrostPostDwellData_t tripleEvapPostDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .secondaryOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellFreezerEvapExitTemperatureInDegFx100 = -1000,
   .secondaryOnlyPostDwellFreezerEvapExitTemperatureInDegFx100 = -1200,
   .postDwellSealedSystemValvePosition = SealedSystemValvePosition_B,
   .postDwellCompressorSpeed = CompressorSpeed_Low,
   .postDwellCondenserFanSpeed = FanSpeed_Low,
   .postDwellFreshFoodDamperPosition = DamperPosition_Closed
};

static const DefrostData_t tripleEvapDefrostData = {
   .idleData = tripleEvapIdleData,
   .prechillPrepData = tripleEvapPrechillPrepData,
   .prechillData = tripleEvapPrechillData,
   .heaterOnEntryData = tripleEvapHeaterOnEntryData,
   .heaterOnData = tripleEvapHeaterOnData,
   .dwellData = tripleEvapDwellData,
   .postDwellData = tripleEvapPostDwellData
};

TEST_GROUP(Defrost_TripleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   PlatformData_t platformData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentTripleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Convertible);
      platformData.numberOfEvaporators = 3;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &tripleEvapDefrostData, &platformData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DefrostStateIs(state);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData, &platformData);
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      DefrostType_t currentDefrostType;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &currentDefrostType);

      switch(state)
      {
         case DefrostHsmState_Idle:
            GivenDefrostStateIs(DefrostState_Idle);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
            break;

         case DefrostHsmState_PrechillPrep:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreezerThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         default:
            break;
      }
   }

   void GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_t convertibleCompartmentStateType)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &convertibleCompartmentStateType);
   }

   void TimeThatPrechillConditionsAreMetInMinutesIs(uint16_t timeThatPrechillConditionsAreMet)
   {
      DataModel_Write(dataModel, Erd_TimeThatPrechillConditionsAreMetInMinutes, &timeThatPrechillConditionsAreMet);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void CompressorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTripleEvapPrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_ConvertibleCompartment);
   }

   void GivenFreezerThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistorDiscovered, set);
   }

   void GivenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }
};

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsFreezerAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCoolingModeChangesToConvertibleCompartmentAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreezer)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_Freezer);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CoolingModeIs(CoolingMode_ConvertibleCompartment);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCoolingModeIsConvertibleCompartmentAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldNotGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAreNotAlreadyMetBecauseCompressorIsOffAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValidAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_Freezer);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCompressorTurnsOnAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(OFF);
   Given CoolingModeIs(CoolingMode_Freezer);

   When CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_TripleEvap, ShouldGoToPrechillWhenCoolingModeChangesToFreezerAndPrechillConditionsMetWhileInPrechillPrepAndConvertibleCompartmentIsActingAsAFreshFood)
{
   GivenConvertibleCompartmentStateTypeIs(ConvertibleCompartmentStateType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given CompressorIs(ON);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);

   When CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
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
   ZeroMinutes = 0
};

enum
{
   FreezerDefrostHeaterMaxOnTimeInMinutes = 45
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
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodThermistorIsValidErd = Erd_FreshFoodThermistor_IsValidResolved,
   .freezerThermistorIsValidErd = Erd_FreezerThermistor_IsValidResolved,
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
   .nextDefrostTypeErd = Erd_NextDefrostType,
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
   .enhancedSabbathIsRequestingDefrostErd = Erd_EnhancedSabbathIsRequestingDefrost
};

static const DefrostIdleData_t idleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR
};

static const DefrostPrechillPrepData_t prechillPrepData = {
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 0,
   .numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 0,
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t prechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .prechillRefrigerantValvePosition = ValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvapFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvapFanSpeed = FanSpeed_Low,
   .prechillIceBoxFanSpeed = FanSpeed_Medium,
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
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open
};

static const DefrostHeaterOnData_t heaterOnData = {
   .defrostHeaterOnRefrigerantValvePosition = ValvePosition_A,
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
   .dwellRefrigerantValvePosition = ValvePosition_B
};

static const DefrostPostDwellData_t postDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .freshFoodOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellFreezerEvapExitTemperatureInDegFx100 = -1000,
   .freshFoodOnlyPostDwellFreezerEvapExitTemperatureInDegFx100 = -1200,
   .postDwellRefrigerantValvePosition = ValvePosition_B,
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

TEST_GROUP(Defrost_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfig, &defrostData);
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
            Given FreezerEvaporatorThermistorValidityIs(Valid);
            Given FreezerThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            When PrechillConditionsAreMet();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;

         case DefrostHsmState_HeaterOnEntry:
            Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
            Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
            Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
            Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

            if(currentDefrostType == DefrostType_FreshFood)
            {
               After(defrostData.prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes * MSEC_PER_MIN);
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

   void PrechillConditionsAreMet()
   {
      CompressorIs(ON);
      CoolingModeIs(CoolingMode_Freezer);
   }

   void PrechillConditionsAreNotMet()
   {
      CompressorIs(OFF);
      CoolingModeIs(CoolingMode_FreshFood);
   }

   void MaxPrechillTimeInMinutesIs(uint8_t maxPrechillTime)
   {
      DataModel_Write(dataModel, Erd_MaxPrechillTimeInMinutes, &maxPrechillTime);
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

   void FreezerEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodDamperPositionVoteShouldBe(DamperPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
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

   void IceCabinetFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
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

   void NextDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_NextDefrostType, &defrostType);
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

      PrechillConditionsAreMet();
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
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPostDwell)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_PostDwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastFreshFoodDefrostWasAbnormal)
{
   Given LastFreshFoodDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastFreezerDefrostWasAbnormal)
{
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndLastConvertibleCompartmentDefrostWasAbnormal)
{
   Given HasConvertibleCompartment();
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerWasTooWarmAtPowerUpAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerWasTooWarmAtPowerUpAndLastFreezerDefrostWasAbnormalAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And LastFreezerDefrostWasAbnormal();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsFalseAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(false);
   And FreezerThermistorValidityIs(Valid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreezerThermistorValidityIs(Valid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And FreezerThermistorValidityIs(Invalid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsValidAndInvalidFreezerEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsValid)
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

TEST(Defrost_SingleEvap, ShouldSetDisableMinimumCompressorOnTimesWhenEnteringHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
}
TEST(Defrost_SingleEvap, ShouldNotSetDisableMinimumCompressorOnTimesWhenEnteringIdle)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvap, ShouldNotSetDisableMinimumCompressorOnTimesWhenEnteringPrechillPrep)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvap, ShouldNotSetDisableMinimumCompressorOnTimesWhenEnteringPrechill)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeInMinutes - 1);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenReadyToDefrostAndLastDefrostsWereNormalAndThermistorsAreValidAndDontSkipPrechillPrepIsNotSet)
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

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenReadyToDefrostAndInvalidFreezerEvapThermistorDuringDefrostIsTrueAndDontSkipPrechillIsSet)
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

TEST(Defrost_SingleEvap, ShouldOnlyGoToPrechillPrepOnceWhenDontSkipPrechillIsSetIfConditionsAreNotPresentForItToGoToPrechillPrepNormallyAndInThisCaseItShouldGoToHeaterOnEntryInsteadOfPrechillPrepNextTime)
{
   Given DefrostIsInPrechillPrepBecauseDontSkipPrechillErdWasSet();

   When DefrostTransitionsFromPrechillPrepToPrechillToHeaterOnEntryToHeaterOnToDwellToPostDwellThenToIdle();
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldDisobeyTheDontSkipPrechillErdWhenFreezerThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Invalid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldDisobeyTheDontSkipPrechillErdWhenFreezerEvaporatorThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And FreezerThermistorValidityIs(Valid);
   And InvalidFreezerEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldClearUseAhamPrechillTimeErdOnEntryToPrechillPrep)
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

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreezerThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given PrechillConditionsAreMet();

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldNotGoToPrechillWhenPrechillConditionsNotMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIs(OFF);
   And CoolingModeIs(CoolingMode_Freezer);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   When CoolingModeIs(CoolingMode_FreshFood);
   And CompressorIs(ON);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillConditionsMetWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillPrepTimerExpiresWhileInPrechillPrep)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   After((defrostData.prechillPrepData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepTheNextTimeThroughIdleAfterItWasTooWarmOnPowerUp)
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

TEST(Defrost_SingleEvap, ShouldExitPostDwellAndGoToIdleAfterFreshFoodOnlyPostDwellExitTimeIfPostDwellExitTimeIsFreshFoodOnly)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.freshFoodOnlyPostDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldExitPostDwellAndGoToIdleAfterPostDwellExitTimeIfCurrentDefrostIsFull)
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

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreezerThermistorIsAlreadyInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldNotGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreezerThermistorIsAlreadyInvalidWhileSabbathModeIsEnabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldNotGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreezerThermistorIsAlreadyInvalidWhileEnhancedSabbathModeIsEnabled)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenEnhancedSabbathModeIs(ENABLED);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenSabbathModeIsDisabledWhileReadyToDefrost)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);
   Given ReadyToDefrost();

   WhenSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnhancedSabbathModeIsDisabledWhileReadyToDefrost)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenEnhancedSabbathModeIs(ENABLED);
   Given ReadyToDefrost();

   WhenEnhancedSabbathModeIs(DISABLED);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenSabbathIsReadyToDefrostWhileSabbathModeIsEnabled)
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

TEST(Defrost_SingleEvap, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileEnhancedSabbathModeIsEnabledAndSabbathModeIsDisabledIfEnhancedSabbathHasNotRequestedADefrost)
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

TEST(Defrost_SingleEvap, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileEnhancedSabbathModeIsEnabledAndSabbathModeIsEnabledIfEnhancedSabbathHasNotRequestedADefrost)
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

TEST(Defrost_SingleEvap, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileSabbathModeIsDisabled)
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

TEST(Defrost_SingleEvap, ShouldStayInIdleWhenSabbathIsReadyToDefrostWhileEnhancedSabbathModeIsDisabled)
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

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenSabbathModeIsEnabledWhileSabbathIsReadyToDefrost)
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

TEST(Defrost_SingleEvap, ShouldStayInIdleWhenEnhancedSabbathModeIsEnabledWhileSabbathIsReadyToDefrostIfEnhancedSabbathHasNotRequestedADefrost)
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

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerThermistorBecomesInvalidWhileInPrechillPrep)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrep)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechillForFreshFoodOnlyDefrost)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   CurrentDefrostTypeShouldBe(DefrostType_FreshFood);

   After(defrostData.prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechillForFullDefrost)
{
   Given NextDefrostTypeIs(DefrostType_Full);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   After(defrostData.prechillData.maxPrechillTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeMinusTimeThatPrechillConditionsAreMetWhileTimeThatPrechillConditionsAreMetIsNotZeroInPrechillForFreshFoodOnlyDefrost)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes - OneMinute);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   CurrentDefrostTypeShouldBe(DefrostType_FreshFood);

   After(OneMinute * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeMinusTimeThatPrechillConditionsAreMetWhileTimeThatPrechillConditionsAreMetIsNotZeroInPrechillForFullDefrost)
{
   Given NextDefrostTypeIs(DefrostType_Full);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeInMinutes - OneMinute);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   CurrentDefrostTypeShouldBe(DefrostType_Full);

   After(OneMinute * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndMaxPrechillTimeAlreadyMet)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndPrechillConditionsHaveBeenMetForLongerThanMaxPrechillTime)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(defrostData.prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerEvapTemperatureGoesBelowExitThreshold)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerEvapTemperatureAlreadyBelowExitThreshold)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerTemperatureGoesBelowExitThreshold)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerTemperatureAlreadyBelowExitThreshold)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureDecreasesToExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldNotGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureDecreasesToExitThresholdAndFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyAtExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillAndStayWhenPrechillConditionsAreMetDuringPrechillPrepWithFreshFoodTemperatureAlreadyAtExitThresholdAndFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureIncreasesToExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100 - 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldNotGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureIncreasesToExitThresholdAndFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100 - 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenPrechillConditionsAreMetDuringPrechillPrepWithFreshFoodTemperatureAlreadyAtExitThresholdAndFreshFoodThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillAndStayWhenPrechillConditionsAreMetDuringPrechillPrepWithFreshFoodTemperatureAlreadyAtExitThresholdButFreshFoodThermistorIsInvalid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FreshFoodThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMaxTempInDegFx100);

   When PrechillConditionsAreMet();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerThermistorBecomesInvalidWhileInPrechill)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FreezerThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechill)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldVoteForLoadsOnEntryToPrechill)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   CompressorSpeedVoteShouldBe(defrostData.prechillData.prechillCompressorSpeed);
   FreezerEvapFanSpeedVoteShouldBe(defrostData.prechillData.prechillFreezerEvapFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData.prechillData.prechillFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldTurnOffCompressorAndFansOnHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodDamperBasedOnParametricallyDefinedPositionOnEntryToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   FreshFoodDamperPositionVoteShouldBe(DamperPosition_Open);
}

TEST(Defrost_SingleEvap, ShouldReleaseControlOfHeaterOnEntryLoadsWhenDefrostIsDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   CompressorSpeedVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheDefrostHeaterAndEnableMinimumCompressorTimesAfterDefrostHeaterOnDelayTimerExpired)
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

TEST(Defrost_SingleEvap, ShouldVoteCompressorAndFansOffAndFreshFoodDamperPositionBasedOnParametricallyDefinedPositionUponEntryToHeaterOn)
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
   FreshFoodDamperPositionVoteShouldBe(DamperPosition_Open);
}

TEST(Defrost_SingleEvap, ShouldReleaseControlOfHeaterOnLoadsWhenDefrostIsDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   CompressorSpeedVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);

   When DisableDefrostIs(false);
   FreezerDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperatureAndFreezerEvaporatorThermistorIsValid)
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

TEST(Defrost_SingleEvap, ShouldNotTurnOffDefrostHeaterAndShouldNotIncrementFreezerDefrostCountAndShouldNotTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperatureAndFreezerEvaporatorThermistorIsInvalid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);
   And NumberOfFreezerDefrostsShouldBe(1);
   And DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
}

TEST(Defrost_SingleEvap, ShouldClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsLessThanFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsValid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes - 1);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldNotClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsEqualToFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsInvalid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitionToDwellStateWhenFreezerHeaterOnTimeReachesMaxOnTime)
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

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountAndSetFreezerDefrostWasAbnormalAndSaveTheFreezerDefrostCountAsTheLastAbnormalFreezerDefrostCountWhenFreezerHeaterOnTimeReachesAbnormalTime)
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

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountJustOnce)
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

TEST(Defrost_SingleEvap, ShouldTurnOffFreezerDefrostHeaterCompressorAndAllFansWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodDamperWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   FreshFoodDamperPositionVoteShouldBe(defrostData.dwellData.dwellFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostDwellAfterDwellTimeHasPassed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvap, ShouldVoteForCompressorAndCondenserFanAndDamperWhenEnteringPostDwellAndOffForFreezerDefrostHeaterAndFreezerEvapFanSpeedAndIceCabinetFanSpeed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   CompressorSpeedVoteShouldBe(defrostData.postDwellData.postDwellCompressorSpeed);
   CondenserFanSpeedVoteShouldBe(defrostData.postDwellData.postDwellCondenserFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData.postDwellData.postDwellFreshFoodDamperPosition);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAfterPostDwellTimeHasPassedForFreshFoodOnlyDefrost)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.freshFoodOnlyPostDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAfterPostDwellTimeHasPassedForFullDefrost)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperatureAndFreezerEvaporatorThermistorIsValid)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperatureAndFreezerEvaporatorThermistorIsInvalid)
{
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperatureAndFreezerEvaporatorThermistorIsValid)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldNotTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperatureAndFreezerEvaporatorThermistorIsInvalid)
{
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvap, ShouldNotCareAboutFreezerDefrostHeaterCompressorAndDamperAndAllFansAndEnableMinimumCompressorTimesWhenEnteringIdleFromPostDwell)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);
   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   FreezerDefrostHeaterVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringIdleOnInit)
{
   Given WaitingToDefrostIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WaitingToDefrostShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringPostDwellOnInit)
{
   Given WaitingToDefrostIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   WaitingToDefrostShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToWaitingToDefrostErdWhenEnteringPostDwellFromDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   WaitingToDefrostShouldBe(false);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   WaitingToDefrostIs(true);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringIdleOnInit)
{
   Given DefrostingIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringPostDwellOnInit)
{
   Given DefrostingIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToDefrostingErdWhenEnteringDwellOnInit)
{
   Given DefrostingIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   DefrostingShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteTrueToDefrostingErdWhenEnteringHeaterOnEntryOnInit)
{
   Given DefrostingIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   DefrostingShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringPostDwellFromDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   DefrostingShouldBe(true);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToWaitingToDefrostErdWhenEnteringPrechillPrepFromIdle)
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

TEST(Defrost_SingleEvap, ShouldTransitionFromWaitingToDefrostStateToDisabledWhenDisableIsRequested)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromDefrostingStateToDisabledWhenDisableIsRequested)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToWaitingToDefrostErdWhenEnteringDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   WaitingToDefrostShouldBe(true);

   When DisableDefrostIs(true);
   WaitingToDefrostShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldWriteFalseToDefrostingErdWhenEnteringDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   DefrostingShouldBe(true);

   When DisableDefrostIs(true);
   DefrostingShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromHeaterOnToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
}

TEST(Defrost_SingleEvap, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromIdleToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
}

TEST(Defrost_SingleEvap, ShouldReleaseControlOfFreezerDefrostHeaterWhenDefrostBecomesEnabledAndTransitionsFromDisabledToIdle)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);

   When DisableDefrostIs(false);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   FreezerDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvap, ShouldWriteToCurrentDefrostTypeWhenExitingWaitingToDefrost)
{
   Given NextDefrostTypeIs(DefrostType_Full);
   And CurrentDefrostTypeIs(DefrostType_FreshFood);
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

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryFromIdleWhenReadyToDefrostAndEepromIsClearedOnStartup)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given ClearedEepromOnStartupIs(true);

   When ReadyToDefrost();
   ClearedEepromOnStartupShouldBe(false);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldSetInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsInvalidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldClearInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsValidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldClearInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldNotClearInvalidFreezerEvaporatorThermistorDuringDefrostWhenTheThermistorIsInvalidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given InvalidFreezerEvaporatorThermistorDuringDefrostIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerDefrostTerminationTemperatureInDegFx100);
   InvalidFreezerEvaporatorThermistorDuringDefrostShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInIdleState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleState)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleStateButTheFreezerThermistorIsInvalid)
{
   Given FreezerThermistorValidityIs(Invalid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInIdleStateButTheFreezerEvaporatorThermistorIsInvalid)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInIdleState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPostDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPostDwellState)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPostDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPrechillPrepState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestWhenIdleTestIsRequestedInHeaterOnState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInHeaterOnState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInHeaterOnState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestAndEnableMinimumCompressorTimesWhenIdleTestIsRequestedInDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToPrechillPrepAndClearTheDefrostTestStateRequestAndEnableMinimumCompressorTimesWhenPrechillTestIsRequestedInDwellState)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldSkipPrechillPrepAndTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestAndNotEnableMinimumCompressorTimesWhenPrechillTestIsRequestedInDwellStateBecauseFreezerEvaporatorThermistorIsInvalid)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldSkipPrechillPrepAndTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenPrechillTestIsRequestedInDwellStateBecauseFreezerThermistorIsInvalid)
{
   Given FreezerThermistorValidityIs(Invalid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInDwellState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToDefrostDwellWhenExitDefrostHeaterOnStateIsRequestedInHeaterOnState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldTransitionToDefrostDwellWhenExitDefrostHeaterOnStateIsRequestedInHeaterOnEntryState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInIdle)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInPrechillPrep)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInPrechill)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInDwell)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInPostDwell)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenExitDefrostHeaterOnStateIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_ExitDefrostHeaterOnState);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenIdleTestIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenDefrostTestIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldDoNothingWhenPrechillTestIsRequestedInDisabledState)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);

   WhenDefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   DefrostTestStateRequestShouldBeNone();
}

TEST(Defrost_SingleEvap, ShouldThrowAssertionWhenFreezerFilteredTemperatureTooWarmOnPowerUpReadyIsSet)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);

   ShouldAssertOnInitialization();
}

TEST(Defrost_SingleEvap, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenRunningAPrechillTestAsFirstFullDefrost)
{
   Given FreezerThermistorValidityIs(true);
   Given FreezerEvaporatorThermistorValidityIs(true);
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenRunningAnIdleTestAfterItExitsIdle)
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

TEST(Defrost_SingleEvap, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenRunningADefrostTestAsFirstFullDefrost)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenDontSkipPrechillPrepFlagIsSet)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldResetFreezerCompartmentWasTooWarmOnPowerUpWhenSabbathIsReadyToDefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);

   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromIdleToHeaterOnEntryWhenSabbathIsReadyToDefrostAndBothSabbathAndEnhancedSabbathModesAreActiveAndEnhancedSabbathIsRequestingADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromIdleToHeaterOnEntryWhenSabbathIsReadyToDefrostAndSabbathModeIsInactiveAndEnhancedSabbathModeIsActiveAndEnhancedSabbathIsRequestingADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldTransitionFromIdleToPrechillPrepWhenSabbathIsReadyToDefrostAndSabbathModeIsActiveAndEnhancedSabbathModeIsInactiveAndEnhancedSabbathIsRequestingADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(DISABLED);

   WhenEnhancedSabbathIsRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldStayInIdleWhenSabbathIsReadyToDefrostAndBothSabbathAndEnhancedSabbathAreActiveAndEnhancedSabbathModeDidNotRequestADefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenEnhancedSabbathIsNotRequestingToDefrost();
   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToHeaterOnEntryWhenEnhancedSabbathIsRequestingADefrostAndSabbathIsAlreadyReadyToDefrostAndEnhancedSabbathIsAlreadyActive)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(ENABLED);

   WhenSabbathIsReadyToDefrost();
   WhenEnhancedSabbathIsRequestingToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

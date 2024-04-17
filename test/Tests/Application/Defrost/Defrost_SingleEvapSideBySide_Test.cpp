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
   OneMinute = 1,
   ZeroMinutes = 0,
};

enum
{
   CaresAbout = Vote_Care,
   DoesNotCareAbout = Vote_DontCare,
};

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 6000 }
};

static TwoDimensionalCalculatedGridLines_t calcGridLines = {
   .firstDimensionGridLines = freshFoodCalcAxis,
   .secondDimensionGridLines = freezerCalcAxis,
   .numberOfGridLinesPerDimension = 6
};

static const DefrostIdleData_t idleData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * MINUTES_PER_HOUR,
   .maxTimeBetweenDefrostsInMinutes = 32 * MINUTES_PER_HOUR,
   .ahamPrechillTimeBetweenDefrostsInMinutes = 6 * MINUTES_PER_HOUR,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 0,
   .numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet = 0
};

static const DefrostPrechillPrepData_t prechillPrepData = {
   .maxPrechillPrepTimeInMinutes = 5
};

static const DefrostPrechillData_t prechillData = {
   .maxPrechillTimeInMinutes = 10,
   .maxPrechillTimeForSecondaryOnlyDefrostInMinutes = 20,
   .prechillSealedSystemValvePosition = SealedSystemValvePosition_B,
   .prechillCompressorSpeed = CompressorSpeed_Low,
   .prechillFreezerEvaporatorFanSpeed = FanSpeed_SuperLow,
   .prechillFreshFoodEvaporatorFanSpeed = FanSpeed_Low,
   .prechillIceCabinetFanSpeed = FanSpeed_Medium,
   .prechillDeliFanSpeed = FanSpeed_High,
   .prechillConvertibleCompartmentDamperPosition = DamperPosition_Open,
   .prechillDeliDamperPosition = DamperPosition_Open,
   .prechillConvertibleCompartmentEvapFanSpeed = FanSpeed_High,
   .prechillFreshFoodDamperPosition = DamperPosition_Closed,
   .prechillFreezerMinTempInDegFx100 = -600,
   .prechillFreshFoodMinTempInDegFx100 = 3200,
   .prechillFreshFoodMaxTempInDegFx100 = 4600,
   .prechillConvertibleCompartmentAsFreezerMinTempInDegFx100 = -600,
   .prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 = 3200,
   .prechillConvertibleCompartmentAsFreshFoodMaxTempInDegFx100 = 4600,
   .prechillPrimaryEvaporatorExitTemperatureInDegFx100 = -3000
};

static const DefrostHeaterOnEntryData_t heaterOnEntryData = {
   .defrostHeaterOnDelayAfterCompressorOffInSeconds = 2,
   .heaterOnEntryFreshFoodDamperPosition = DamperPosition_Open,
   .heaterOnEntryConvertibleCompartmentDamperPosition = DamperPosition_Closed,
   .heaterOnEntrySealedSystemValvePosition = SealedSystemValvePosition_A
};

static const DefrostHeaterData_t freshFoodHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 60,
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .defrostTerminationTemperatureInDegFx100 = 4460
};

static const DefrostHeaterData_t freezerHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 60,
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 10,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 32,
   .defrostTerminationTemperatureInDegFx100 = 5900
};

static DefrostConvertibleCompartmentCommonHeaterData_t convertibleCompartmentHeater = {
   .defrostHeaterMaxOnTimeInMinutes = 45,
   .defrostTerminationTemperatureInDegFx100 = 4460
};

static const DefrostConvertibleCompartmentStateDependentHeaterData_t convertibleCompartmentHeaterAsFreshFood = {
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 2,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 21,
};

static const DefrostConvertibleCompartmentStateDependentHeaterData_t convertibleCompartmentHeaterAsFreezer = {
   .invalidThermistorDefrostHeaterMaxOnTimeInMinutes = 25,
   .heaterOnTimeToSetAbnormalDefrostInMinutes = 42,
};

static const DefrostHeaterOnData_t heaterOnData = {
   .freshFoodHeater = freshFoodHeater,
   .freezerHeater = freezerHeater,
   .convertibleCompartmentHeater = convertibleCompartmentHeater,
   .convertibleCompartmentHeaterAsFreshFood = convertibleCompartmentHeaterAsFreshFood,
   .convertibleCompartmentHeaterAsFreezer = convertibleCompartmentHeaterAsFreezer
};
static const DefrostDwellData_t dwellData = {
   .dwellTimeInMinutes = 7,
   .dwellFreshFoodDamperPosition = DamperPosition_Closed,
   .dwellSealedSystemValvePosition = SealedSystemValvePosition_B
};

static const DefrostPostDwellData_t postDwellData = {
   .postDwellExitTimeInMinutes = 5,
   .secondaryOnlyPostDwellExitTimeInMinutes = 10,
   .postDwellPrimaryEvaporatorExitTemperatureInDegFx100 = -1000,
   .secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100 = -1200,
   .postDwellSealedSystemValvePosition = SealedSystemValvePosition_B,
   .postDwellCompressorSpeed = CompressorSpeed_Low,
   .postDwellCondenserFanSpeed = FanSpeed_Low,
   .postDwellFreshFoodDamperPosition = DamperPosition_Closed,
   .postDwellConvertibleCompartmentDamperPosition = DamperPosition_Closed,
   .postDwellDeliDamperPosition = DamperPosition_Closed
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
   EnhancedSabbathData_t enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_FreshFood);
      BITMAP_SET(platformData.compartmentBitmap.bitmap, Compartment_Freezer);
      platformData.numberOfEvaporators = 1;

      enhancedSabbathData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost = 0;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void DefrostIsInitializedWithoutAssertErdsSet()
   {
      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, clear);

      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
   }

   void GivenDefrostIsInitializedWithEepromNotClearedOnStartup()
   {
      Defrost_Init(&instance, dataModel, &defrostConfiguration, &defrostData, &platformData, &enhancedSabbathData);
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
            GivenCabinetIsNotTooWarmOnPowerUp();
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

   void CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_CabinetFilteredTemperatureTooWarmOnPowerUp, &actualState);

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

   void DeliFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_DeliFanSpeed_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void DeliFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_DeliFanSpeed_DefrostVote, &actual);

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

   void ConvertibleCompartmentDamperPositionVoteShouldBe(DamperPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDamperPosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void ConvertibleCompartmentDamperPositionVoteShouldBeDontCare()
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDamperPosition_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void DeliDamperPositionVoteShouldBe(DamperPosition_t expected)
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_DeliDamperPosition_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void DeliDamperPositionVoteShouldBeDontCare()
   {
      DamperVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_DeliDamperPosition_DefrostVote, &actual);

      CHECK_FALSE(actual.care);
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

   void FreezerDefrostHeaterVoteShouldBe(HeaterState_t expected, Vote_t care)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_EQUAL(care, actual.care);
   }

   void ConvertibleCompartmentDefrostHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote;
      vote.state = state;
      vote.care = Vote_Care;
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &vote);
   }

   void ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_t expected, Vote_t care)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_ConvertibleCompartmentDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_EQUAL(care, actual.care);
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

   void InvalidEvaporatorThermistorDuringDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_InvalidEvaporatorThermistorDuringDefrost, &state);
   }

   void InvalidEvaporatorThermistorDuringDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_InvalidEvaporatorThermistorDuringDefrost, &actual);
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
      And FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1);
      And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
      And FreezerEvaporatorThermistorValidityIs(Valid);
      And InvalidEvaporatorThermistorDuringDefrostIs(true);
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

      When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

      GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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

   void FreshFoodDefrostHeaterVoteShouldBe(HeaterState_t expected, Vote_t care)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostHeater_DefrostVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_EQUAL(care, actual.care);
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

   void CalculatedGridLinesAre(TwoDimensionalCalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &gridLines);
   }

   void WhenConvertibleCompartmentCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenConvertibleCompartmentCabinetTemperatureIs(temperature);
   }

   void GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions()
   {
      Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
      Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
      Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
      GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
      Given FreezerEvaporatorThermistorValidityIs(Valid);
      Given FreezerThermistorValidityIs(Valid);
      Given FreshFoodThermistorValidityIs(Valid);
      GivenConvertibleCompartmentCabinetThermistorValidityIs(Valid);
      Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);
   }

   void GivenCabinetIsNotTooWarmOnPowerUp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1);
      Given CalculatedGridLinesAre(calcGridLines);

      CHECK_TRUE((freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] - 1) > (defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1));
   }

   void GivenCabinetIsTooWarmOnPowerUp()
   {
      Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
      Given CalculatedGridLinesAre(calcGridLines);

      CHECK_TRUE((defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100) < (freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh]));
   }

   void GivenDefrostPowerUpReadyErdIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DefrostPowerUpReady, &state);
   }

   void DefrostPowerUpReadyErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DefrostPowerUpReady, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NextDefrostTypeOverrideShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostTypeOverride, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfSecondaryOnlyDefrostsShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_NumberOfSecondaryOnlyDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenAllThermistorsValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_FreshFoodEvapThermistor_IsValidResolved, &state);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved, &state);
   }

   void GivenAllEvapThermistorsAreAtTemp(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100, &temp);
      DataModel_Write(dataModel, Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100, &temp);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100, &temp);
   }

   void GivenDefrostStartsInDwellAndDwellTimerExpires()
   {
      GivenCabinetIsNotTooWarmOnPowerUp();
      And DefrostStateIs(DefrostState_Dwell);
      And DefrostIsInitialized();
      DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
      After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   }

   void FilteredFreshFoodEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FilteredConvertibleCompartmentEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenFilteredFreezerEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      FilteredFreezerEvapTemperatureIs(temperature);
   }

   void WhenFilteredFreshFoodEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      FilteredFreshFoodEvapTemperatureIs(temperature);
   }

   void WhenFilteredConvertibleCompartmentEvapTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      FilteredConvertibleCompartmentEvapTemperatureIs(temperature);
   }

   void WhenFreezerEvapThermistorHasBeenDiscovered()
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorDiscovered, set);
   }
};

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToTrueWhenFreezerFilteredTemperatureIsAboveGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetCabinetFilteredTemperatureTooWarmOnPowerUpReadyErdToTrueAfterInitializedAndAfterCabinetTooWarmErdIsSet)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Valid);
   GivenDefrostPowerUpReadyErdIs(false);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);

   DefrostPowerUpReadyErdShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToFalseWhenFreezerFilteredTemperatureIsLessThanGridFreezerExtremeHysteresisAndFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 - 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToTrueWhenFreezerFilteredTemperatureIsAboveFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToTrueWhenFreezerFilteredTemperatureIsEqualToFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(true);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToFalseWhenFreezerThermistorIsInvalidAndFreezerFilteredTemperatureIsAboveGridFreezerExtremeHysteresis)
{
   Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Invalid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToFalseWhenFreezerThermistorIsInvalidAndFreezerFilteredTemperatureIsAboveFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100 + 1);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Invalid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSetErdToFalseWhenFreezerThermistorIsInvalidAndFreezerFilteredTemperatureIsEqualToFreezerDefrostTerminationTemperature)
{
   Given FilteredFreezerCabinetTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   And CalculatedGridLinesAre(calcGridLines);
   And FreezerThermistorValidityIs(Invalid);
   And DefrostIsInitialized();

   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoDwellHsmStateWhenCabinetFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoHeaterOnHsmStateWhenCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoHeaterOnHsmStateWhenCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoDwellHsmStateWhenCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPostDwell)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostStateIs(DefrostState_PostDwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And DefrostStateIs(DefrostState_Disabled);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDisabled)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
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

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndCabinetWasTooWarmAtPowerUpAndResetFreezerWasTooWarmErdToFalse)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndCabinetWasTooWarmAtPowerUpAndLastFreezerDefrostWasAbnormalAndResetFreezerWasTooWarmErdToFalse)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   And LastFreezerDefrostWasAbnormal();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndInvalidEvaporatorThermistorDuringDefrostIsFalseAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   And FreezerThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidEvaporatorThermistorDuringDefrostIs(false);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToPrechillPrepWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasNotBeenDiscoveredAndInvalidEvaporatorThermistorDuringDefrostIsFalseAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   And FreezerThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidEvaporatorThermistorDuringDefrostIs(false);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndInvalidEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(Valid);
   GivenFreezerThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalidAndHasBeenDiscoveredAndInvalidEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And InvalidEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsValidAndInvalidEvaporatorThermistorDuringDefrostIsTrueAndFreezerThermistorIsValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   And FreezerEvaporatorThermistorValidityIs(Valid);
   And InvalidEvaporatorThermistorDuringDefrostIs(true);

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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And InvalidEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   And DontSkipPrechillPrepErdShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldOnlyGoToPrechillPrepOnceWhenDontSkipPrechillIsSetIfConditionsAreNotPresentForItToGoToPrechillPrepNormallyAndInThisCaseItShouldGoToHeaterOnEntryInsteadOfPrechillPrepNextTime)
{
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillData.prechillFreshFoodMinTempInDegFx100 + 1);
   GivenConvertibleCompartmentCabinetTemperatureIs(defrostData.prechillData.prechillConvertibleCompartmentAsFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInPrechillPrepBecauseDontSkipPrechillErdWasSet();

   When DefrostTransitionsFromPrechillPrepToPrechillToHeaterOnEntryToHeaterOnToDwellToPostDwellThenToIdle();
   And InvalidEvaporatorThermistorDuringDefrostIs(true);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisobeyTheDontSkipPrechillErdWhenFreezerThermistorIsInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   And InvalidEvaporatorThermistorDuringDefrostIs(true);
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisobeyTheDontSkipPrechillErdWhenFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   GivenCabinetIsNotTooWarmOnPowerUp();
   GivenFreezerEvapThermistorHasBeenDiscovered();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);
   And FreezerThermistorValidityIs(Valid);
   And InvalidEvaporatorThermistorDuringDefrostIs(true);
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
   GivenCabinetIsNotTooWarmOnPowerUp();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And InvalidEvaporatorThermistorDuringDefrostIs(true);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   GivenCabinetIsTooWarmOnPowerUp();
   And FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreezerThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   ClearReadyToDefrost();

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(defrostData.dwellData.dwellTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(defrostData.postDwellData.postDwellExitTimeInMinutes * MSEC_PER_MIN);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvapSideBySide, ShouldExitPostDwellAndGoToIdleAfterPostDwellExitTimeIfCurrentDefrostIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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

TEST(Defrost_SingleEvapSideBySide, ShouldNotGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidThenDiscoveredAndValidWhileInPrechillPrep)
{
   Given FreezerThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   GivenFreezerEvapThermistorHasNotBeenDiscovered();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   WhenFreezerEvapThermistorHasBeenDiscovered();
   When FreezerEvaporatorThermistorValidityIs(Valid);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
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

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 2);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerEvapTemperatureAlreadyBelowExitThresholdAndFreezerEvapThermistorIsValid)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FreshFoodThermistorValidityIs(Invalid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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
   FreezerEvapFanSpeedVoteShouldBe(defrostData.prechillData.prechillFreezerEvaporatorFanSpeed);
   FreshFoodEvapFanSpeedVoteShouldBe(defrostData.prechillData.prechillFreshFoodEvaporatorFanSpeed);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(defrostData.prechillData.prechillConvertibleCompartmentEvapFanSpeed);
   IceCabinetFanSpeedVoteShouldBe(defrostData.prechillData.prechillIceCabinetFanSpeed);
   DeliFanSpeedVoteShouldBe(defrostData.prechillData.prechillDeliFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData.prechillData.prechillFreshFoodDamperPosition);
   ConvertibleCompartmentDamperPositionVoteShouldBe(defrostData.prechillData.prechillConvertibleCompartmentDamperPosition);
   DeliDamperPositionVoteShouldBe(defrostData.prechillData.prechillDeliDamperPosition);
   SealedSystemValvePositionVoteShouldBe(defrostData.prechillData.prechillSealedSystemValvePosition);
   CondenserFanSpeedVoteShouldBe(defrostData.prechillData.prechillCondenserFanSpeed);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTurnOffCompressorAndFansOnHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   DeliFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteForDampersAndSealedSystemValveOnEntryToHeaterOnEntry)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   FreshFoodDamperPositionVoteShouldBe(defrostData.heaterOnEntryData.heaterOnEntryFreshFoodDamperPosition);
   ConvertibleCompartmentDamperPositionVoteShouldBe(DamperPosition_Closed);
   DeliDamperPositionVoteShouldBe(DamperPosition_Closed);
   SealedSystemValvePositionVoteShouldBe(defrostData.heaterOnEntryData.heaterOnEntrySealedSystemValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldReleaseControlOfHeaterOnEntryLoadsWhenDefrostIsDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);
   CompressorSpeedVoteShouldBeDontCare();
   CondenserFanSpeedVoteShouldBeDontCare();
   FreezerEvapFanSpeedVoteShouldBeDontCare();
   FreshFoodEvapFanSpeedVoteShouldBeDontCare();
   ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare();
   IceCabinetFanSpeedVoteShouldBeDontCare();
   DeliFanSpeedVoteShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
   ConvertibleCompartmentDamperVoteShouldBeDontCare();
   DeliDamperPositionVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvapSideBySide, ShouldExitOnHeaterEntryStateAndTurnOnTheDefrostHeaterAndEnableMinimumCompressorTimesAfterDefrostHeaterOnDelayTimerExpired)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteCompressorAndFansOffAndDampersAndValveBasedOnParametricallyDefinedPositionUponEntryToHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData.heaterOnEntryData.defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   DeliFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodDamperPositionVoteShouldBe(defrostData.heaterOnEntryData.heaterOnEntryFreshFoodDamperPosition);
   ConvertibleCompartmentDamperVoteShouldBe(DamperPosition_Closed);
   DeliDamperPositionVoteShouldBe(DamperPosition_Closed);
   SealedSystemValvePositionVoteShouldBe(defrostData.heaterOnEntryData.heaterOnEntrySealedSystemValvePosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldReleaseControlOfHeaterOnLoadsWhenDefrostIsDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);

   When DisableDefrostIs(true);
   DefrostHsmStateShouldBe(DefrostHsmState_Disabled);

   CompressorSpeedVoteShouldBeDontCare();
   CondenserFanSpeedVoteShouldBeDontCare();
   FreezerEvapFanSpeedVoteShouldBeDontCare();
   FreshFoodEvapFanSpeedVoteShouldBeDontCare();
   ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare();
   IceCabinetFanSpeedVoteShouldBeDontCare();
   DeliFanSpeedVoteShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
   ConvertibleCompartmentDamperVoteShouldBeDontCare();
   DeliDamperPositionVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();

   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

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
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfFreezerDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotTurnOffDefrostHeaterAndShouldNotIncrementFreezerDefrostCountAndShouldNotTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperatureAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);
   And NumberOfFreezerDefrostsShouldBe(1);
   And DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearFreezerDefrostWasAbnormalAndFreezerDefrostHeaterOnForMaxTimeFaultWhenFreezerEvaporatorReachesTerminationTemperatureAndFreezerHeaterOnTimeIsLessThanFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsValid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes - 1);
   GivenFreezerDefrostHeaterOnForMaxTimeFaultIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotClearFreezerDefrostWasAbnormalWhenFreezerEvaporatorReachesTerminationTemperatureAndFreezerHeaterOnTimeIsEqualToFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   GivenFreezerThermistorHasBeenDiscovered();
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotClearFreezerDefrostWasAbnormalWhenFreezerEvaporatorReachesTerminationTemperatureAndFreezerHeaterOnTimeIsEqualToFreezerHeaterOnTimeToSetAbnormalDefrostAndFreezerEvaporatorThermistorIsValid)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitionToDwellStateWhenFreezerHeaterOnTimeReachesMaxOnTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerDefrostsIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes - 1);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   And NumberOfFreezerDefrostsShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountAndSetFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeReachesAbnormalTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostsIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given LastFreezerDefrostWasNormal();
   Given NumberOfFreezerDefrostsIs(3);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostsShouldBe(2);
   And FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountJustOnce)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostsIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On, Vote_Care);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostsShouldBe(2);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.heaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfFreezerAbnormalDefrostsShouldBe(2);
}

TEST(Defrost_SingleEvapSideBySide, ShouldDisableMinimumCompressorTimesAndRequestValveHomingAndVoteForDwellLoadsWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   DisableMinimumCompressorTimesShouldBe(SET, Vote_Care);
   SealedSystemValveHomingRequestShouldBe(SET);

   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   DeliFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodDamperPositionVoteShouldBe(defrostData.dwellData.dwellFreshFoodDamperPosition);
   ConvertibleCompartmentDamperPositionVoteShouldBe(DamperPosition_Closed);
   DeliDamperPositionVoteShouldBe(DamperPosition_Closed);
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

   CompressorSpeedVoteShouldBe(defrostData.postDwellData.postDwellCompressorSpeed);
   CondenserFanSpeedVoteShouldBe(defrostData.postDwellData.postDwellCondenserFanSpeed);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ConvertibleCompartmentEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
   DeliFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodDamperPositionVoteShouldBe(defrostData.postDwellData.postDwellFreshFoodDamperPosition);
   ConvertibleCompartmentDamperPositionVoteShouldBe(defrostData.postDwellData.postDwellConvertibleCompartmentDamperPosition);
   DeliDamperPositionVoteShouldBe(defrostData.postDwellData.postDwellDeliDamperPosition);
   SealedSystemValvePositionVoteShouldBe(defrostData.postDwellData.postDwellSealedSystemValvePosition);
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

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperatureAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperatureAndFreezerEvaporatorThermistorIsValid)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperatureAndFreezerEvaporatorThermistorIsInvalidAndHasBeenDiscovered)
{
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotCareAboutPostDwellLoadsAndEnableMinimumCompressorTimesWhenEnteringIdleFromPostDwell)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);
   When FilteredFreezerEvapTemperatureIs(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   DisableMinimumCompressorTimesShouldBe(CLEAR, Vote_DontCare);
   CompressorSpeedVoteShouldBeDontCare();
   CondenserFanSpeedVoteShouldBeDontCare();
   FreezerDefrostHeaterVoteShouldBeDontCare();
   FreshFoodDefrostHeaterVoteShouldBeDontCare();
   ConvertibleCompartmentDefrostHeaterVoteShouldBeDontCare();
   FreezerEvapFanSpeedVoteShouldBeDontCare();
   FreshFoodEvapFanSpeedVoteShouldBeDontCare();
   ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare();
   IceCabinetFanSpeedVoteShouldBeDontCare();
   DeliFanSpeedVoteShouldBeDontCare();
   FreshFoodDamperVoteShouldBeDontCare();
   ConvertibleCompartmentDamperPositionVoteShouldBeDontCare();
   DeliDamperPositionVoteShouldBeDontCare();
   SealedSystemValvePositionVoteShouldBeDontCare();
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
   GivenCabinetIsNotTooWarmOnPowerUp();
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
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteFreezerDefrostHeaterOffWhenTransitioningFromIdleToDisabled)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given FreezerDefrostHeaterVoteIs(HeaterState_On);
   Given FreshFoodDefrostHeaterIsVoted(HeaterState_On);
   Given ConvertibleCompartmentDefrostHeaterVoteIs(HeaterState_On);

   When DisableDefrostIs(true);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   FreshFoodDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
   ConvertibleCompartmentDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);
}

TEST(Defrost_SingleEvapSideBySide, ShouldReleaseControlOfFreezerDefrostHeaterWhenDefrostBecomesEnabledAndTransitionsFromDisabledToIdle)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Disabled);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off, Vote_Care);

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
   GivenCabinetIsNotTooWarmOnPowerUp();
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

TEST(Defrost_SingleEvapSideBySide, ShouldSetInvalidEvaporatorThermistorDuringDefrostAndNotSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsInvalidAndHasBeenDiscoveredAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndSetFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndMaxOnTimeReachesOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData.heaterOnData.freezerHeater.defrostHeaterMaxOnTimeInMinutes);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(SET);
}

TEST(Defrost_SingleEvapSideBySide, ShouldClearInvalidEvaporatorThermistorDuringDefrostAndFreezerDefrostHeaterOnForMaxTimeFaultWhenTheThermistorIsValidAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);
   GivenFreezerDefrostHeaterOnForMaxTimeFaultIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(CLEAR);
   FreezerDefrostHeaterOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotClearInvalidEvaporatorThermistorDuringDefrostWhenTheThermistorIsInvalidAndHasBeenDiscoveredAndTheThermistorTemperatureReachesHeaterOnTerminationTemperatureOnHeaterOn)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   GivenFreezerEvapThermistorHasBeenDiscovered();
   Given FreezerEvaporatorThermistorValidityIs(Invalid);
   Given InvalidEvaporatorThermistorDuringDefrostIs(SET);

   When FilteredFreezerEvapTemperatureIs(defrostData.heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100);
   InvalidEvaporatorThermistorDuringDefrostShouldBe(SET);
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

TEST(Defrost_SingleEvapSideBySide, ShouldTransitDefrost_ionToHeaterOnEntryAndClearTheDefrostTestStateRequestWhenDefrostTestIsRequestedInIdleState)
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

TEST(Defrost_SingleEvapSideBySide, ShouldTransitionToIdleAndClearTheDefrostTestStateRequestAndVoteDontCareForPrechillLoadsWhenIdleTestIsRequestedInPrechillState)
{
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   GivenDefrostIsInitializedAndInPrechillWithAllThermistorsValidAndNotMeetingPrechillConditions();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostTestStateRequestShouldBeNone();
   SealedSystemValvePositionVoteShouldBeDontCare();
   FreshFoodDamperPositionVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBeDontCare();
   CondenserFanSpeedVoteShouldBeDontCare();
   FreezerEvapFanSpeedVoteShouldBeDontCare();
   FreshFoodEvapFanSpeedVoteShouldBeDontCare();
   IceCabinetFanSpeedVoteShouldBeDontCare();
   ConvertibleCompartmentDamperPositionVoteShouldBeDontCare();
   ConvertibleCompartmentEvapFanSpeedVoteShouldBeDontCare();
   DeliDamperPositionVoteShouldBeDontCare();
   DeliFanSpeedVoteShouldBeDontCare();
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

TEST(Defrost_SingleEvapSideBySide, ShouldResetCabinetWasTooWarmOnPowerUpWhenRunningAPrechillTestAsFirstFullDefrost)
{
   Given FreezerThermistorValidityIs(true);
   Given FreezerEvaporatorThermistorValidityIs(true);
   GivenCabinetIsTooWarmOnPowerUp();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Prechill);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetCabinetWasTooWarmOnPowerUpWhenRunningAnIdleTestAfterItExitsIdle)
{
   Given FreezerThermistorValidityIs(true);
   Given FreezerEvaporatorThermistorValidityIs(true);
   GivenCabinetIsTooWarmOnPowerUp();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Idle); // when requesting Idle test while in Idle, it exits and reenters Idle
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetCabinetWasTooWarmOnPowerUpWhenRunningADefrostTestAsFirstFullDefrost)
{
   GivenCabinetIsTooWarmOnPowerUp();
   Given FreezerThermistorValidityIs(true);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostTestIsRequested(DefrostTestStateRequest_Defrost);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetCabinetWasTooWarmOnPowerUpWhenDontSkipPrechillPrepFlagIsSet)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   And DontSkipPrechillErdIs(SET);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvapSideBySide, ShouldResetCabinetWasTooWarmOnPowerUpWhenSabbathIsReadyToDefrost)
{
   GivenLastDefrostsWereNormalFreezerFilteredTemperatureWasTooWarmOnPowerUpThermistorsAreValidAndDefrostIsInIdleState();
   GivenSabbathModeIs(ENABLED);

   WhenSabbathIsReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
   CabinetFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
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
   GivenCabinetIsTooWarmOnPowerUp();
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_Prechill);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_HeaterOn);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldInitializeIntoIdleHsmStateWhenEepromClearedOnStartupIsSetAndCabinetFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   GivenCabinetIsNotTooWarmOnPowerUp();
   GivenClearedEepromOnStartupIs(true);
   GivenDefrostStateIs(DefrostState_Dwell);
   GivenDefrostIsInitializedWithEepromNotClearedOnStartup();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteWithPrechillFreshFoodDamperPositionParameterWhenPrechillConditionsAreMet)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   WhenPrechillConditionsAreMet();
   FreshFoodDamperPositionVoteShouldBe(defrostData.prechillData.prechillFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvapSideBySide, ShouldVoteWithSealedSystemValvePositionParameterWhenPrechillConditionsAreMet)
{
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
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

TEST(Defrost_SingleEvapSideBySide, ShouldSetCurrentDefrostTypeToDefrostFullOnInit)
{
   CurrentDefrostTypeIs(DefrostType_DontCare);

   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(Defrost_SingleEvapSideBySide, ShouldNotUpdateDefrostTypeUponExitingIdleState)
{
   Given NextDefrostTypeOverrideIs(DefrostType_DontCare);
   Given CurrentDefrostTypeIs(DefrostType_DontCare);
   GivenDefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
   NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsFull)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldSkipPostDwellIfAnyPrimaryEvaporatorTempIsAtOrBelowPostDwellPrimaryEvapExitTempAndCurrentDefrostTypeIsSecondary)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.secondaryOnlyPostDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldOnlyExitPostDwellWhenFreshFoodEvaporatorTempChangesToAtOrBelowPostDwellPrimaryEvapExitTemp)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   GivenAllThermistorsValidityIs(Valid);
   GivenAllEvapThermistorsAreAtTemp(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   GivenDefrostStartsInDwellAndDwellTimerExpires();
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreshFoodEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   WhenFilteredFreezerEvapTemperatureChangesTo(defrostData.postDwellData.postDwellPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenPrimaryEvaporatorTemperatureChangesToMeetExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldGoToHeaterOnEntryWhenPrimaryEvaporatorTemperatureChangesToBeEqualToTheExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvapSideBySide, ShouldRemainInPrechillWhenPrimaryEvaporatorTemperatureChangesToBeMoreThanTheExitConditionsWhileInPrechillState)
{
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillData.prechillPrimaryEvaporatorExitTemperatureInDegFx100 + 10);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

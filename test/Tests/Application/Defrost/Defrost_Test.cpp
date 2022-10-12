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
   DontCare = false,
   Care = true
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
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .readyToDefrost = Erd_ReadyToDefrost,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .freezerEvaporatorThermistorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .freshFoodThermistorIsValidErd = Erd_FreshFood_ThermistorIsValid,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .disableMinimumTimeRequestErd = Erd_DisableMinimumCompressorTimes,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_DefrostVote,
   .freezerEvapFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_DefrostVote,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .freezerEvaporatorFilteredTemperatureResolvedErd = Erd_FreezerEvap_FilteredTemperatureResolved,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .freshFoodFilteredTemperatureResolvedErd = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerDefrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .numberOfFreezerAbnormalDefrostCycleCountErd = Erd_NumberOfFreezerAbnormalDefrostCycles,
   .freezerDefrostHeaterOnTimeInMinutesErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .timerModuleErd = Erd_TimerModule
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 16 * MINUTES_PER_HOUR
};

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 5500 }
};

static CalculatedGridLines_t calcGridLines = {
   .freshFoodGridLine = freshFoodCalcAxis,
   .freezerGridLine = freezerCalcAxis
};

TEST_GROUP(Defrost_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Defrost_t instance;
   const DefrostData_t *defrostData;
   const GridData_t *gridData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      gridData = PersonalityParametricData_Get(dataModel)->gridData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
   }

   void FilteredFreshFoodCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolved, &temperature);
   }

   void FilteredFreezerEvapTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreezerEvap_FilteredTemperatureResolved, &temperature);
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
            Given FreshFoodThermistorValidityIs(Valid);

            When ReadyToDefrost();
            DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
            break;

         case DefrostHsmState_Prechill:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

            When CompressorIsOn();
            DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
            break;

         case DefrostHsmState_HeaterOnEntry:
            Given MaxPrechillTimeInMinutesIs(TenMinutes);
            Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
            Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
            Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
            Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

            After(TenMinutes * MSEC_PER_MIN);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
            break;

         case DefrostHsmState_HeaterOn:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

            After(defrostData->defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
            break;

         case DefrostHsmState_Dwell:
            Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
            And DefrostStateIs(DefrostState_Dwell);
            And DefrostIsInitialized();

            DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
            break;

         case DefrostHsmState_PostDwell:
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

            After(defrostData->dwellTimeInMinutes * MSEC_PER_MIN);
            DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
            break;

         default:
            break;
      }
   }

   void NothingShouldHappen()
   {
   }

   void SealedSystemValveIsInPosition(ValvePosition_t position)
   {
      ValveVotedPosition_t vote = {
         .position = position,
         .care = true
      };

      DataModel_Write(dataModel, Erd_ValvePosition_ResolvedVote, &vote);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFood_ThermistorIsValid, &state);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void ReadyToDefrost()
   {
      DataModel_Write(dataModel, Erd_ReadyToDefrost, set);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }

   void FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void CompressorIsOn()
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, on);
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

   void DamperVoteShouldBeDontCare()
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

   void DisableMinimumCompressorTimesShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableMinimumCompressorTimes, &actual);

      CHECK_EQUAL(expected, actual);
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

   void FreezerDefrostCycleCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostCycleCount, &count);
   }

   void FreezerDefrostCycleCountShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_FreezerDefrostCycleCount, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void NumberOfFreezerAbnormalDefrostCycleCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreezerAbnormalDefrostCycles, &count);
   }

   void NumberOfFreezerAbnormalDefrostCycleCountShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreezerAbnormalDefrostCycles, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerAbnormalDefrostCycleCountShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_FreezerAbnormalDefrostCycleCount, &actual);

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
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Idle);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(true);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasPrechill)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Prechill);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoHeaterOnHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasHeaterOn)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_HeaterOn);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoDwellHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasDwell)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Dwell);
   And DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldInitializeIntoIdleHsmStateWhenFreezerFilteredTemperatureIsNotTooWarmAtPowerUpAndPreviousDefrostStateWasIdle)
{
   Given FreezerFilteredTemperatureTooWarmAtPowerUpIs(false);
   And DefrostStateIs(DefrostState_Idle);
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

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenReadyToDefrostAndFreezerEvapThermistorIsInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   And FreezerEvaporatorThermistorValidityIs(Invalid);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenReadyToDefrostAndLastDefrostsWereNormalAndThermistorsAreValid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMetAndThermistorsAreValid)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given FreshFoodThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIsOn();

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillConditionsMetWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillPrepTimerExpiresWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   After((defrostData->maxPrechillPrepTimeInMinutes * MSEC_PER_MIN) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillPrepAndFreshFoodThermistorIsAlreadyInvalid)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And FreshFoodThermistorValidityIs(Invalid);
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When ReadyToDefrost();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreshFoodThermistorBecomesInvalidWhileInPrechillPrep)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreshFoodThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechillPrep)
{
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   After(TenMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeMinusTimeThatPrechillConditionsAreMetWhileTimeThatPrechillConditionsAreMetIsNotZeroInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(NineMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   After(OneMinute * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndMaxPrechillTimeAlreadyMet)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(TenMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndPrechillConditionsHaveBeenMetForLongerThanMaxPrechillTime)
{
   Given MaxPrechillTimeInMinutesIs(TwoHundredFiftyFiveMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ThreeHundredMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerEvapTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerEvapTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureGoesAboveExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMaxTempInDegFx100 - 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyAboveExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMaxTempInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreshFoodThermistorBecomesInvalidWhileInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FreshFoodThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenFreezerEvapThermistorBecomesInvalidWhileInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given FreshFoodThermistorValidityIs(Valid);
   Given FreezerEvaporatorThermistorValidityIs(Valid);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldVoteForLoadsOnEntryToPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData->prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData->prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData->prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   CompressorSpeedVoteShouldBe(defrostData->prechillCompressorSpeed);
   FreezerEvapFanSpeedVoteShouldBe(defrostData->prechillFreezerEvapFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData->prechillFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldTurnOffCompressorAndFansAndNotCareAboutDamperVoteOnHeaterOnEntryState)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   DamperVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldExitOnHeaterEntryStateAndTurnOnTheDefrostHeaterAfterDefrostHeaterOnDelayTimerExpired)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);

   After(defrostData->defrostHeaterOnDelayAfterCompressorOffInSeconds * MSEC_PER_SEC - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerDefrostHeaterVoteShouldBeDontCare();

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOn);
   And FreezerDefrostHeaterVoteShouldBe(HeaterState_On);
}

TEST(Defrost_SingleEvap, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitToDwellStateWhenFreezerEvaporatorTemperatureReachesFreezerDefrostTerminationTemperature)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostCycleCountIs(1);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FilteredFreezerEvapTemperatureIs(defrostData->freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   And FreezerDefrostCycleCountShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsLessThanFreezerHeaterOnTimeToSetAbnormalDefrost)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes - 1);

   When FilteredFreezerEvapTemperatureIs(defrostData->freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(CLEAR);
}

TEST(Defrost_SingleEvap, ShouldNotClearFreezerDefrostWasAbnormalWhenFreezerHeaterOnTimeIsEqualToFreezerHeaterOnTimeToSetAbnormalDefrost)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given LastFreezerDefrostWasAbnormal();
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);

   When FilteredFreezerEvapTemperatureIs(defrostData->freezerDefrostTerminationTemperatureInDegFx100);
   FreezerDefrostWasAbnormalFlagShouldBe(SET);
}

TEST(Defrost_SingleEvap, ShouldTurnOffDefrostHeaterAndIncrementFreezerDefrostCountAndTransitionToDwellStateWhenFreezerHeaterOnTimeReachesMaxOnTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given FreezerDefrostCycleCountIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   And FreezerDefrostCycleCountShouldBe(2);
   And DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountAndSetFreezerDefrostWasAbnormalAndSaveTheFreezerDefrostCountAsTheLastAbnormalFreezerDefrostCountWhenFreezerHeaterOnTimeReachesAbnormalTime)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostCycleCountIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   Given LastFreezerDefrostWasNormal();
   Given FreezerDefrostCycleCountIs(3);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostCycleCountShouldBe(2);
   And FreezerDefrostWasAbnormalFlagShouldBe(SET);
   And FreezerAbnormalDefrostCycleCountShouldBe(3);
}

TEST(Defrost_SingleEvap, ShouldIncrementNumberOfFreezerAbnormalDefrostCycleCountJustOnce)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOn);
   Given NumberOfFreezerAbnormalDefrostCycleCountIs(1);
   Given FreezerDefrostHeaterMaxOnTimeInMinutesIs(FreezerDefrostHeaterMaxOnTimeInMinutes);
   FreezerDefrostHeaterVoteShouldBe(HeaterState_On);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes);
   NumberOfFreezerAbnormalDefrostCycleCountShouldBe(2);

   When FreezerDefrostHeaterOnTimeInMinutesIs(defrostData->freezerHeaterOnTimeToSetAbnormalDefrostInMinutes + 1);
   NumberOfFreezerAbnormalDefrostCycleCountShouldBe(2);
}

TEST(Defrost_SingleEvap, ShouldTurnOffFreezerDefrostHeaterCompressorAndAllFansWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   FreezerDefrostHeaterVoteShouldBe(HeaterState_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   IceCabinetFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(Defrost_SingleEvap, ShouldVoteForFreshFoodDamperWhenEnteringDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);
   FreshFoodDamperPositionVoteShouldBe(defrostData->dwellFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldTransitionToPostDwellAfterDwellTimeHasPassed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Dwell);

   After(defrostData->dwellTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);
}

TEST(Defrost_SingleEvap, ShouldVoteForCompressorAndCondenserFanAndDamperAndDisableMinimumCompressorTimesWhenEnteringPostDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   CompressorSpeedVoteShouldBe(defrostData->postDwellCompressorSpeed);
   CondenserFanSpeedVoteShouldBe(defrostData->postDwellCondenserFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData->postDwellFreshFoodDamperPosition);
   DisableMinimumCompressorTimesShouldBe(true);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleAfterPostDwellTimeHasPassed)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   After(defrostData->postDwellExitTimeInMinutes * MSEC_PER_MIN - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PostDwell);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsLessThanExitTemperature)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData->postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldTransitionToIdleWhenFreezerEvaporatorTemperatureIsEqualToExitTemperature)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);

   When FilteredFreezerEvapTemperatureIs(defrostData->postDwellFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldNotCareAboutFreezerDefrostHeaterCompressorAndDamperAndAllFansAndEnableMinimumCompressorTimesWhenEnteringIdleFromPostDwell)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PostDwell);
   When FilteredFreezerEvapTemperatureIs(defrostData->postDwellFreezerEvapExitTemperatureInDegFx100 - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);

   FreezerDefrostHeaterVoteShouldBeDontCare();
   CompressorSpeedVoteShouldBeDontCare();
   DamperVoteShouldBeDontCare();
   FanSpeedVotesShouldBeDontCare();
   DisableMinimumCompressorTimesShouldBe(false);
}

TEST_GROUP(Defrost_DualEvap)
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

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void DefrostIsInitialized()
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady, set);
      Defrost_Init(&instance, dataModel, &defrostConfig);
   }
};

TEST(Defrost_DualEvap, ShouldInitialize)
{
   DefrostIsInitialized();
}

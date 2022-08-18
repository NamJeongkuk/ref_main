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
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"
#include "GridData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

#define PowerUpDelayInMs(_gridPeriodicRunRateInMSec) (5 * _gridPeriodicRunRateInMSec)

enum
{
   INVALID = false,
   VALID = true
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

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostStateErd = Erd_DefrostState,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .defrostReadyTimerIsSatisfied = Erd_DefrostReadyTimerIsSatisfied,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .freezerFilteredTemperatureTooWarmOnPowerUpReadyErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady,
   .maxPrechillTimeInMinutesErd = Erd_MaxPrechillTimeInMinutes,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .freezerFanSpeedVoteErd = Erd_FreezerFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .freezerEvaporatorFilteredTemperatureResolvedErd = Erd_FreezerEvap_FilteredTemperatureResolved,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .freshFoodFilteredTemperatureResolvedErd = Erd_FreshFood_FilteredTemperatureResolved,
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
   PersonalityParametricData_t personalityParametricData;
   Defrost_t instance;
   DefrostData_t defrostData;
   GridData_t gridData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);

      GridData_TestDouble_Init(&gridData);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetSabbath(&personalityParametricData, &sabbathData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &singleEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
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

            When DefrostReadyTimerIsSatisfied();
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
            Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
            Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
            Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
            Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

            After(TenMinutes * MSEC_PER_MIN);
            DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
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

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void DefrostReadyTimerIsSatisfied()
   {
      DataModel_Write(dataModel, Erd_DefrostReadyTimerIsSatisfied, set);
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

   void FreezerFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerFanSpeed_DefrostVote, &actual);

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
      DataModel_Read(dataModel, Erd_FreezerFanSpeed_DefrostVote, &fanVote);

      DamperVotedPosition_t damperVote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &damperVote);

      CHECK_FALSE(compressorVote.care);
      CHECK_FALSE(fanVote.care);
      CHECK_FALSE(damperVote.care);
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

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndLastFreshFoodDefrostWasAbnormal)
{
   Given LastFreshFoodDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndLastFreezerDefrostWasAbnormal)
{
   Given LastFreezerDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndLastConvertibleCompartmentDefrostWasAbnormal)
{
   Given LastConvertibleCompartmentDefrostWasAbnormal();
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndFreezerWasTooWarmAtPowerUpAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenDefrostReadyTimerIsSatisfiedAndFreezerWasTooWarmAtPowerUpAndLastFreezerDefrostWasAbnormalAndResetFreezerWasTooWarmErdToFalse)
{
   Given FreezerFilteredTemperatureTooWarmOnPowerUpIs(true);
   And LastFreezerDefrostWasAbnormal();
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
   FreezerFilteredTemperatureTooWarmOnPowerUpShouldBe(false);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenDefrostReadyTimerIsSatisfiedAndLastDefrostsWereNormal)
{
   Given LastFreshFoodDefrostWasNormal();
   And LastFreezerDefrostWasNormal();
   And LastConvertibleCompartmentDefrostWasNormal();
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenEnteringPrechillPrepAndPrechillConditionsAlreadyMet)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);
   Given CompressorIsOn();

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillConditionsMetWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillWhenPrechillPrepTimerExpiresWhileInPrechillPrep)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);

   After((defrostData.maxPrechillPrepTimeInMinutes * MSEC_PER_MIN) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Prechill);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryAfterMaxPrechillTimeWhileTimeThatPrechillConditionsAreMetIsZeroInPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
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
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerEvapTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreezerTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreezerTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureGoesBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyBelowExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenInPrechillAndFreshFoodTemperatureGoesAboveExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMaxTempInDegFx100 - 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   When FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMaxTempInDegFx100);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnEntryWhenEnteringPrechillAndFreshFoodTemperatureAlreadyAboveExitThreshold)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_PrechillPrep);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMaxTempInDegFx100);

   When CompressorIsOn();
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldVoteForLoadsOnEntryToPrechill)
{
   Given MaxPrechillTimeInMinutesIs(TenMinutes);
   Given TimeThatPrechillConditionsAreMetInMinutesIs(ZeroMinutes);
   Given FilteredFreezerEvapTemperatureIs(defrostData.prechillFreezerEvapExitTemperatureInDegFx100 + 1);
   Given FilteredFreezerCabinetTemperatureIs(defrostData.prechillFreezerMinTempInDegFx100 + 1);
   Given FilteredFreshFoodCabinetTemperatureIs(defrostData.prechillFreshFoodMinTempInDegFx100 + 1);
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_Prechill);

   CompressorSpeedVoteShouldBe(defrostData.prechillCompressorSpeed);
   FreezerFanSpeedVoteShouldBe(defrostData.prechillFreezerFanSpeed);
   FreshFoodDamperPositionVoteShouldBe(defrostData.prechillFreshFoodDamperPosition);
}

TEST(Defrost_SingleEvap, ShouldVoteDontCareOnExitOfPrechill)
{
   Given DefrostIsInitializedAndStateIs(DefrostHsmState_HeaterOnEntry);
   PrechillLoadVotesShouldBeDontCare();
}

TEST_GROUP(Defrost_DualEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   Defrost_t instance;
   DefrostData_t defrostData;
   GridData_t gridData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);

      GridData_TestDouble_Init(&gridData);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetSabbath(&personalityParametricData, &sabbathData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &dualEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
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

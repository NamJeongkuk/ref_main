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

static const DefrostConfiguration_t defrostConfig = {
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .freezerFilteredTemperatureResolvedErd = Erd_Freezer_FilteredTemperatureResolved,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .defrostStateErd = Erd_DefrostState,
   .numberOfFreezerAbnormalDefrostCyclesErd = Erd_NumberofFreezerAbnormalDefrostCycles,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .freezerAbnormalDefrostCycleCountErd = Erd_FreezerAbnormalDefrostCycleCount,
   .freshFoodDefrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .freezerDefrostCycleCountErd = Erd_FreezerDefrostCycleCount,
   .defrostReadyTimerIsSatisfied = Erd_DefrostReadyTimerIsSatisfied,
   .freezerFilteredTemperatureWasTooWarmOnPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp,
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
      Defrost_Init(&instance, dataModel, &defrostConfig);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void FreezerAbnormalDefrostCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_NumberofFreezerAbnormalDefrostCycles, &count);
   }

   void FreezerAbnormalDefrostCountShouldBe(uint16_t expectedCount)
   {
      uint16_t actualCount;
      DataModel_Read(dataModel, Erd_NumberofFreezerAbnormalDefrostCycles, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
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

   void LastFreezerDefrostShouldBeAbnormal()
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreezerDefrostWasAbnormal, &actualState);

      CHECK_EQUAL(true, actualState);
   }

   void LastAbnormalFreezerDefrostCycleWas(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerAbnormalDefrostCycleCount, &count);
   }

   void LastAbnormalFreezerDefrostCycleShouldBe(uint16_t expectedCount)
   {
      uint16_t actualCount;
      DataModel_Read(dataModel, Erd_FreezerAbnormalDefrostCycleCount, &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void CurrentDefrostCountIs(uint16_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostCycleCount, &count);
   }

   void DefrostInitializedWithFreezerTempAboveExtremeHysteresis()
   {
      Given FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostInitializedWithNormalFreezerCabinetTemperatures()
   {
      Given FilteredFreezerCabinetTemperatureIs(1000);
      Given CalculatedGridLinesAre(calcGridLines);
      Given DefrostIsInitialized();
   }

   void DefrostIsInitializedAndStateIs(DefrostHsmState_t state)
   {
      switch(state)
      {
         case DefrostHsmState_Idle:
            Given DefrostStateIs(DefrostState_Idle);
            Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

            After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
            DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

            After(1);
            DefrostHsmStateShouldBe(DefrostHsmState_Idle);
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
};

TEST(Defrost_SingleEvap, ShouldInitializeIntoPowerUpHsmState)
{
   Given CalculatedGridLinesAre(calcGridLines);
   Given DefrostIsInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);
}

TEST(Defrost_SingleEvap, ShouldGoToDwellWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateWasHeaterOnAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_HeaterOn);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Dwell);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsPrechillAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Prechill);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsIdleAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Idle);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsDwellAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Dwell);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndDefrostStateIsDisabledAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldIncrementFreezerAbnormalDefrostCountWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   FreezerAbnormalDefrostCountShouldBe(0);

   After(1);
   FreezerAbnormalDefrostCountShouldBe(1);
}

TEST(Defrost_SingleEvap, ShouldSetLastFreezerDefrostAsAbnormalWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   LastFreezerDefrostWasNormal();

   After(1);
   LastFreezerDefrostShouldBeAbnormal();
}

TEST(Defrost_SingleEvap, ShouldSaveLastFreezerAbnormalDefrostCountWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   LastAbnormalFreezerDefrostCycleShouldBe(32);

   After(1);
   LastAbnormalFreezerDefrostCycleShouldBe(35);
}

TEST(Defrost_SingleEvap, ShouldSetLastFreezerDefrostAsAbnormalWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasNormalAndLastFreezerDefrostCycleCountThatWasAbnormalEqualsCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasNormal();
   Given LastAbnormalFreezerDefrostCycleWas(35);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   LastFreezerDefrostWasNormal();

   After(1);
   LastFreezerDefrostShouldBeAbnormal();
}

TEST(Defrost_SingleEvap, ShouldSaveLastFreezerAbnormalDefrostCountWhenFilteredFreezerCabinetTemperatureIsGreaterThanExtremeHysteresisAndLastFreezerDefrostWasAbnormalAndLastFreezerDefrostCycleCountThatWasAbnormalDoesNotEqualCurrentDefrostCountAfterPowerUpDelay)
{
   Given FreezerAbnormalDefrostCountIs(0);
   Given LastFreezerDefrostWasAbnormal();
   Given LastAbnormalFreezerDefrostCycleWas(32);
   Given CurrentDefrostCountIs(35);
   Given DefrostInitializedWithFreezerTempAboveExtremeHysteresis();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   LastAbnormalFreezerDefrostCycleShouldBe(32);

   After(1);
   LastAbnormalFreezerDefrostCycleShouldBe(35);
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
   And FreezerEvaporatorThermistorValidityIs(VALID);
   And CompressorStateIs(CompressorState_MinimumOffTime);
   And FreezerFilteredTemperatureTooWarmOnPowerUpIs(false);
   And DefrostIsInitializedAndStateIs(DefrostHsmState_Idle);

   When DefrostReadyTimerIsSatisfied();
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToPrechillPrepWhenLastFreezerDefrostWasNormalAndDefrostStateWasPrechillAfterPowerUpDelay)
{
   Given FreezerEvaporatorThermistorValidityIs(VALID);
   Given DefrostStateIs(DefrostState_Prechill);
   Given CompressorStateIs(CompressorState_MinimumOffTime);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_PrechillPrep);
}

TEST(Defrost_SingleEvap, ShouldGoToHeaterOnWhenLastFreezerDefrostWasNormalAndDefrostStateWasHeaterOnAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_HeaterOn);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_HeaterOnEntry);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenLastFreezerDefrostWasNormalAndDefrostStateWasDwellAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Dwell);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
}

TEST(Defrost_SingleEvap, ShouldGoToIdleWhenLastFreezerDefrostWasNormalAndDefrostStateWasDisabledAfterPowerUpDelay)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given DefrostInitializedWithNormalFreezerCabinetTemperatures();

   After(PowerUpDelayInMs(gridData.gridPeriodicRunRateInMSec) - 1);
   DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

   After(1);
   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
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
      Defrost_Init(&instance, dataModel, &defrostConfig);
   }
};


/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "GridLineCalculator.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define And

enum
{
   AFreshFoodOffsetTemperature = 10,
   AnotherFreshFoodOffsetTemperature = 10 * 2,

   AFreezerOffsetTemperature = 5,
   AnotherFreezerOffsetTemperature = 5 * 2,

   AFreshFoodShiftTemperature = 40,
   AnotherFreshFoodShiftTemperature = 40 * 2,

   AFreezerShiftTemperature = 20,
   AnotherFreezerShiftTemperature = 20 * 2,

   AFreshFoodSetpointTemperature = 12,
   AnotherFreshFoodSetpointTemperature = 12 * 2,

   AFreezerSetpointTemperature = 6,
   AnotherFreezerSetpointTemperature = 6 * 2,

   AFreshFoodAdjustedSetpointTemperature = 9,
   AnotherFreshFoodAdjustedSetpointTemperature = 9 * 2,

   AFreezerAdjustedSetpointTemperature = 18,
   AnotherFreezerAdjustedSetpointTemperature = 18 * 2,

   ACrossAmbientHysteresisAdjustmentInDegFx100 = 23,
};

static const GridLineAdjustmentErds_t freshFoodGridLineAdjustmentErds = {
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .offsetInDegFx100Erd = Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
   .thermalShiftInDegFx100Erd = Erd_FreshFood_ThermalShiftInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_FreshFood_AdjustedSetpointInDegFx100
};

static const GridLineAdjustmentErds_t freezerGridLineAdjustmentErds = {
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .offsetInDegFx100Erd = Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
   .thermalShiftInDegFx100Erd = Erd_Freezer_ThermalShiftInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_Freezer_AdjustedSetpointInDegFx100,
};

static const GridLineCalculatorConfiguration_t config = {
   .calculatedGridLineErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .gridLineAdjustmentErds = {
      freshFoodGridLineAdjustmentErds,
      freezerGridLineAdjustmentErds,
   }
};

TEST_GROUP(GridLineCalculator)
{
   GridLineCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      GivenTheAdjustedSetpointPluginReadyIs(true);
   }

   void GivenTheModuleIsInitialized()
   {
      GridLineCalculator_Init(
         &instance,
         &config,
         dataModel);
   }

   void WhenTheFreshFoodAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void WhenTheFreezerAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void GivenTheAdjustedSetpointsAre(
      TemperatureDegFx100_t freshFoodAdjustedSetpoint,
      TemperatureDegFx100_t freezerAdjustedSetpoint)
   {
      WhenTheFreshFoodAdjustedSetpointIs(freshFoodAdjustedSetpoint);
      WhenTheFreezerAdjustedSetpointIs(freezerAdjustedSetpoint);
   }

   void WhenTheFreshFoodSumOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
         &offset);
   }

   void WhenTheFreezerSumOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
         &offset);
   }

   void GivenTheOffsetsAre(
      TemperatureDegFx100_t freshFoodOffset,
      TemperatureDegFx100_t freezerOffset)
   {
      WhenTheFreshFoodSumOffsetIs(freshFoodOffset);
      WhenTheFreezerSumOffsetIs(freezerOffset);
   }

   void GivenTheFreezerThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_ThermalShiftInDegFx100,
         &shift);
   }

   void WhenTheFreezerThermalShiftIs(TemperatureDegFx100_t shift)
   {
      GivenTheFreezerThermalShiftIs(shift);
   }

   void WhenTheCrossAmbientHysteresisAdjustmentChangesTo(TemperatureDegFx100_t adjustment)
   {
      DataModel_Write(
         dataModel,
         Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
         &adjustment);
   }

   void GivenTheFreshFoodThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_ThermalShiftInDegFx100,
         &shift);
   }

   void WhenTheFreshFoodThermalShiftIs(TemperatureDegFx100_t shift)
   {
      GivenTheFreshFoodThermalShiftIs(shift);
   }

   void GivenTheShiftsAre(
      TemperatureDegFx100_t freshFoodShift,
      TemperatureDegFx100_t freezerShift)
   {
      GivenTheFreshFoodThermalShiftIs(freshFoodShift);
      GivenTheFreezerThermalShiftIs(freezerShift);
   }

   void WhenTheFreshFoodRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperatureInDegFx100 = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &temp);
   }

   void WhenTheFreezerRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperatureInDegFx100 = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreezerSetpoint_ResolvedVote,
         &temp);
   }

   void GivenTheRawSetpointsAre(
      TemperatureDegFx100_t freshFoodRawSetpoint,
      TemperatureDegFx100_t freezerRawSetpoint)
   {
      WhenTheFreshFoodRawSetpointIs(freshFoodRawSetpoint);
      WhenTheFreezerRawSetpointIs(freezerRawSetpoint);
   }

   void GivenTheGridLineCalculationErdsAreInitialized()
   {
      GivenTheAdjustedSetpointsAre(AFreshFoodAdjustedSetpointTemperature,
         AFreezerAdjustedSetpointTemperature);

      GivenTheOffsetsAre(AFreshFoodOffsetTemperature,
         AFreezerOffsetTemperature);

      GivenTheRawSetpointsAre(AFreshFoodSetpointTemperature,
         AFreezerSetpointTemperature);

      GivenTheShiftsAre(AFreshFoodShiftTemperature,
         AFreezerShiftTemperature);
   }

   void GivenTheAdjustedSetpointPluginReadyIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, &state);
   }

   void TheCalculatedGridLineTempShouldBe(
      TemperatureDegFx100_t temperature,
      uint8_t dimension,
      uint8_t gridLineIndex)
   {
      CalculatedGridLines_t calcLines;
      DataModel_Read(dataModel,
         Erd_Grid_CalculatedGridLines,
         &calcLines);

      if(dimension == GridDelta_FreshFood)
      {
         CHECK_EQUAL(temperature,
            calcLines.freshFoodGridLine.gridLinesDegFx100[gridLineIndex]);
      }
      else
      {
         CHECK_EQUAL(temperature,
            calcLines.freezerGridLine.gridLinesDegFx100[gridLineIndex]);
      }
   }

   void TheGridLineTemperaturesShouldBeInitializedValues()
   {
      TheCalculatedGridLineTempShouldBe(
         (0 + AFreshFoodOffsetTemperature),
         GridDelta_FreshFood,
         GridLine_Nfl);

      And TheCalculatedGridLineTempShouldBe(
         (-450 + AFreshFoodAdjustedSetpointTemperature),
         GridDelta_FreshFood,
         GridLine_FreshFoodLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         (150 + AFreshFoodShiftTemperature),
         GridDelta_FreshFood,
         GridLine_FreshFoodSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         (450 + AFreshFoodSetpointTemperature),
         GridDelta_FreshFood,
         GridLine_FreshFoodHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         (950 + AFreshFoodAdjustedSetpointTemperature),
         GridDelta_FreshFood,
         GridLine_FreshFoodExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         (1150 + AFreshFoodAdjustedSetpointTemperature),
         GridDelta_FreshFood,
         GridLine_FreshFoodSuperHigh);

      And TheCalculatedGridLineTempShouldBe(
         (-250 + AFreezerAdjustedSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         (0 + AFreezerOffsetTemperature),
         GridDelta_Freezer,
         GridLine_FreezerSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         (250 + AFreezerSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         (600 + AFreezerShiftTemperature),
         GridDelta_Freezer,
         GridLine_FreezerExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         (750 + AFreezerAdjustedSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerSuperHigh);

      And TheCalculatedGridLineTempShouldBe(
         (5500 + AFreezerOffsetTemperature),
         GridDelta_Freezer,
         GridLine_FreezerExtremeHigh);
   }
   void TheFreezerGridLineTemperaturesShouldBeInitializedValues()
   {
      TheCalculatedGridLineTempShouldBe(
         (-250 + AFreezerAdjustedSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         (0 + AFreezerOffsetTemperature),
         GridDelta_Freezer,
         GridLine_FreezerSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         (250 + AFreezerSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         (600 + AFreezerShiftTemperature),
         GridDelta_Freezer,
         GridLine_FreezerExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         (750 + AFreezerAdjustedSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerSuperHigh);

      And TheCalculatedGridLineTempShouldBe(
         (5500 + AFreezerOffsetTemperature),
         GridDelta_Freezer,
         GridLine_FreezerExtremeHigh);
   }

   void TheFreshFoodGridLineTemperaturesShouldBeAdjustedWithCrossAmbientHysteresis()
   {
      TheCalculatedGridLineTempShouldBe(
         (0 + AFreshFoodOffsetTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100),
         GridDelta_FreshFood,
         GridLine_Nfl);

      And TheCalculatedGridLineTempShouldBe(
         (-450 + AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100),
         GridDelta_FreshFood,
         GridLine_FreshFoodLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         (150 + AFreshFoodShiftTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100),
         GridDelta_FreshFood,
         GridLine_FreshFoodSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         (450 + AFreshFoodSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100),
         GridDelta_FreshFood,
         GridLine_FreshFoodHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         (950 + AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100),
         GridDelta_FreshFood,
         GridLine_FreshFoodExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         (1150 + AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100),
         GridDelta_FreshFood,
         GridLine_FreshFoodSuperHigh);
   };
};

TEST(GridLineCalculator, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(GridLineCalculator, ShouldAssertWhenAdjustedSetpointPluginIsNotReadyOnInit)
{
   GivenTheAdjustedSetpointPluginReadyIs(false);
   ShouldFailAssertion(GivenTheModuleIsInitialized());
}

TEST(GridLineCalculator, ShouldCalculateGridLinesOnInitIncludingAdjustmentErds)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();
   TheGridLineTemperaturesShouldBeInitializedValues();
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenOffsetChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheFreshFoodSumOffsetIs(AnotherFreshFoodOffsetTemperature);
   WhenTheFreezerSumOffsetIs(AnotherFreezerOffsetTemperature);

   TheCalculatedGridLineTempShouldBe(
      (0 + AnotherFreshFoodOffsetTemperature),
      GridDelta_FreshFood,
      GridLine_Nfl);

   And TheCalculatedGridLineTempShouldBe((0 + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerSetpointDelta);

   And TheCalculatedGridLineTempShouldBe(
      (750 + AFreezerAdjustedSetpointTemperature),
      GridDelta_Freezer,
      GridLine_FreezerSuperHigh);

   And TheCalculatedGridLineTempShouldBe(
      (5500 + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerExtremeHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenSetpointChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   TheGridLineTemperaturesShouldBeInitializedValues();

   WhenTheFreshFoodRawSetpointIs(AnotherFreshFoodSetpointTemperature);
   WhenTheFreezerRawSetpointIs(AnotherFreezerSetpointTemperature);

   TheCalculatedGridLineTempShouldBe(
      (450 + AnotherFreshFoodSetpointTemperature),
      GridDelta_FreshFood,
      GridLine_FreshFoodHighHyst);

   TheCalculatedGridLineTempShouldBe(
      (250 + AnotherFreezerSetpointTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerHighHyst);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenAdjustedSetpointChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheFreshFoodAdjustedSetpointIs(AnotherFreshFoodAdjustedSetpointTemperature);
   WhenTheFreezerAdjustedSetpointIs(AnotherFreezerAdjustedSetpointTemperature);

   TheCalculatedGridLineTempShouldBe(
      (-450 + AnotherFreshFoodAdjustedSetpointTemperature),
      GridDelta_FreshFood,
      GridLine_FreshFoodLowHyst);

   TheCalculatedGridLineTempShouldBe(
      (950 + AnotherFreshFoodAdjustedSetpointTemperature),
      GridDelta_FreshFood,
      GridLine_FreshFoodExtraHigh);

   TheCalculatedGridLineTempShouldBe(
      (1150 + AnotherFreshFoodAdjustedSetpointTemperature),
      GridDelta_FreshFood,
      GridLine_FreshFoodSuperHigh);

   TheCalculatedGridLineTempShouldBe(
      (-250 + AnotherFreezerAdjustedSetpointTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerLowHyst);

   TheCalculatedGridLineTempShouldBe(
      (750 + AnotherFreezerAdjustedSetpointTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerSuperHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenFreshFoodShiftChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheFreshFoodThermalShiftIs(AnotherFreshFoodShiftTemperature);

   TheCalculatedGridLineTempShouldBe(
      (150 + AnotherFreshFoodShiftTemperature),
      GridDelta_FreshFood,
      GridLine_FreshFoodSetpointDelta);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenFreezerShiftChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheFreezerThermalShiftIs(AnotherFreezerShiftTemperature);

   And TheCalculatedGridLineTempShouldBe(
      (600 + AnotherFreezerShiftTemperature),
      GridDelta_Freezer,
      GridLine_FreezerExtraHigh);
}

TEST(GridLineCalculator, ShouldRecalculateFreshFoodGridLinesWhenCrossAmbientHysteresisAdjustmentChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheCrossAmbientHysteresisAdjustmentChangesTo(ACrossAmbientHysteresisAdjustmentInDegFx100);

   TheFreshFoodGridLineTemperaturesShouldBeAdjustedWithCrossAmbientHysteresis();
   TheFreezerGridLineTemperaturesShouldBeInitializedValues();
}


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
#define Then
#define When
#define Given

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

      AdjustedSetpointPluginReadyIs(true);
   }

   void TheModuleIsInitialized()
   {
      GridLineCalculator_Init(
         &instance,
         &config,
         dataModel);
   }

   void FreshFoodAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void FreezerAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void AdjustedSetpointsAre(
      TemperatureDegFx100_t freshFoodAdjustedSetpoint,
      TemperatureDegFx100_t freezerAdjustedSetpoint)
   {
      FreshFoodAdjustedSetpointIs(freshFoodAdjustedSetpoint);
      FreezerAdjustedSetpointIs(freezerAdjustedSetpoint);
   }

   void FreshFoodSumOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
         &offset);
   }

   void FreezerSumOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
         &offset);
   }

   void OffsetsAre(
      TemperatureDegFx100_t freshFoodOffset,
      TemperatureDegFx100_t freezerOffset)
   {
      FreshFoodSumOffsetIs(freshFoodOffset);
      FreezerSumOffsetIs(freezerOffset);
   }

   void FreezerThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_ThermalShiftInDegFx100,
         &shift);
   }

   void FreshFoodThermalShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_ThermalShiftInDegFx100,
         &shift);
   }

   void ShiftsAre(
      TemperatureDegFx100_t freshFoodShift,
      TemperatureDegFx100_t freezerShift)
   {
      FreshFoodThermalShiftIs(freshFoodShift);
      FreezerThermalShiftIs(freezerShift);
   }

   void FreshFoodRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperatureInDegFx100 = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &temp);
   }

   void FreezerRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperatureInDegFx100 = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreezerSetpoint_ResolvedVote,
         &temp);
   }

   void RawSetpointsAre(
      TemperatureDegFx100_t freshFoodRawSetpoint,
      TemperatureDegFx100_t freezerRawSetpoint)
   {
      FreshFoodRawSetpointIs(freshFoodRawSetpoint);
      FreezerRawSetpointIs(freezerRawSetpoint);
   }

   void TheGridLineCalculationErdsAreInitialized()
   {
      AdjustedSetpointsAre(AFreshFoodAdjustedSetpointTemperature,
         AFreezerAdjustedSetpointTemperature);

      OffsetsAre(AFreshFoodOffsetTemperature,
         AFreezerOffsetTemperature);

      RawSetpointsAre(AFreshFoodSetpointTemperature,
         AFreezerSetpointTemperature);

      ShiftsAre(AFreshFoodShiftTemperature,
         AFreezerShiftTemperature);
   }

   void AdjustedSetpointPluginReadyIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, &state);
   }

   void CalculatedGridLineTempShouldBe(
      TemperatureDegFx100_t temperature,
      uint8_t dimension,
      uint8_t gridLineIndex)
   {
      CalculatedGridLines_t calcLines;
      DataModel_Read(dataModel,
         Erd_Grid_CalculatedGridLines,
         &calcLines);

      if(dimension == FreshFoodGridLineDimension)
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

   void GridLineTemperaturesShouldBeInitializedValues()
   {
      CalculatedGridLineTempShouldBe(
         (0 + AFreshFoodOffsetTemperature),
         FreshFoodGridLineDimension,
         GridLine_Nfl);

      And CalculatedGridLineTempShouldBe(
         (-450 + AFreshFoodAdjustedSetpointTemperature),
         FreshFoodGridLineDimension,
         GridLine_FreshFoodLowHyst);

      And CalculatedGridLineTempShouldBe(
         (150 + AFreshFoodShiftTemperature),
         FreshFoodGridLineDimension,
         GridLine_FreshFoodLowHystDelta);

      And CalculatedGridLineTempShouldBe(
         (450 + AFreshFoodSetpointTemperature),
         FreshFoodGridLineDimension,
         GridLine_FreshFoodHighHyst);

      And CalculatedGridLineTempShouldBe(
         (950 + AFreshFoodAdjustedSetpointTemperature),
         FreshFoodGridLineDimension,
         GridLine_FreshFoodExtraHigh);

      And CalculatedGridLineTempShouldBe(
         (1150 + AFreshFoodAdjustedSetpointTemperature),
         FreshFoodGridLineDimension,
         GridLine_FreshFoodSuperHigh);

      And CalculatedGridLineTempShouldBe(
         (-250 + AFreezerAdjustedSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerLowHyst);

      And CalculatedGridLineTempShouldBe(
         (0 + AFreezerOffsetTemperature),
         GridDelta_Freezer,
         GridLine_FreezerDelta);

      And CalculatedGridLineTempShouldBe(
         (250 + AFreezerSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerHighHyst);

      And CalculatedGridLineTempShouldBe(
         (600 + AFreezerShiftTemperature),
         GridDelta_Freezer,
         GridLine_FreezerExtraHigh);

      And CalculatedGridLineTempShouldBe(
         (750 + AFreezerAdjustedSetpointTemperature),
         GridDelta_Freezer,
         GridLine_FreezerSuperHigh);

      And CalculatedGridLineTempShouldBe(
         (5500 + AFreezerOffsetTemperature),
         GridDelta_Freezer,
         GridLine_FreezerExtremeHigh);
   }
};

TEST(GridLineCalculator, ShouldInitialize)
{
   Given TheModuleIsInitialized();
}

TEST(GridLineCalculator, ShouldAssertWhenAdjustedSetpointPluginIsNotReadyOnInit)
{
   Given AdjustedSetpointPluginReadyIs(false);
   ShouldFailAssertion(TheModuleIsInitialized());
}

TEST(GridLineCalculator, ShouldCalculateGridLinesOnInitIncludingAdjustmentErds)
{
   Given TheGridLineCalculationErdsAreInitialized();
   And TheModuleIsInitialized();
   GridLineTemperaturesShouldBeInitializedValues();
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenOffsetChanges)
{
   Given TheGridLineCalculationErdsAreInitialized();
   And TheModuleIsInitialized();

   When FreshFoodSumOffsetIs(AnotherFreshFoodOffsetTemperature);
   And FreezerSumOffsetIs(AnotherFreezerOffsetTemperature);

   CalculatedGridLineTempShouldBe(
      (0 + AnotherFreshFoodOffsetTemperature),
      FreshFoodGridLineDimension,
      GridLine_Nfl);

   And CalculatedGridLineTempShouldBe((0 + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerDelta);

   And CalculatedGridLineTempShouldBe(
      (750 + AFreezerAdjustedSetpointTemperature),
      GridDelta_Freezer,
      GridLine_FreezerSuperHigh);

   And CalculatedGridLineTempShouldBe(
      (5500 + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerExtremeHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenSetpointChanges)
{
   Given TheGridLineCalculationErdsAreInitialized();
   And TheModuleIsInitialized();

   GridLineTemperaturesShouldBeInitializedValues();

   When FreshFoodRawSetpointIs(AnotherFreshFoodSetpointTemperature);
   And FreezerRawSetpointIs(AnotherFreezerSetpointTemperature);

   CalculatedGridLineTempShouldBe(
      (450 + AnotherFreshFoodSetpointTemperature),
      FreshFoodGridLineDimension,
      GridLine_FreshFoodHighHyst);

   CalculatedGridLineTempShouldBe(
      (250 + AnotherFreezerSetpointTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerHighHyst);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenAdjustedSetpointChanges)
{
   Given TheGridLineCalculationErdsAreInitialized();
   And TheModuleIsInitialized();

   When FreshFoodAdjustedSetpointIs(AnotherFreshFoodAdjustedSetpointTemperature);
   And FreezerAdjustedSetpointIs(AnotherFreezerAdjustedSetpointTemperature);

   CalculatedGridLineTempShouldBe(
      (-450 + AnotherFreshFoodAdjustedSetpointTemperature),
      FreshFoodGridLineDimension,
      GridLine_FreshFoodLowHyst);

   CalculatedGridLineTempShouldBe(
      (950 + AnotherFreshFoodAdjustedSetpointTemperature),
      FreshFoodGridLineDimension,
      GridLine_FreshFoodExtraHigh);

   CalculatedGridLineTempShouldBe(
      (1150 + AnotherFreshFoodAdjustedSetpointTemperature),
      FreshFoodGridLineDimension,
      GridLine_FreshFoodSuperHigh);

   CalculatedGridLineTempShouldBe(
      (-250 + AnotherFreezerAdjustedSetpointTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerLowHyst);

   CalculatedGridLineTempShouldBe(
      (750 + AnotherFreezerAdjustedSetpointTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerSuperHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenFreshFoodShiftChanges)
{
   Given TheGridLineCalculationErdsAreInitialized();
   And TheModuleIsInitialized();

   When FreshFoodThermalShiftIs(AnotherFreshFoodShiftTemperature);

   CalculatedGridLineTempShouldBe(
      (150 + AnotherFreshFoodShiftTemperature),
      FreshFoodGridLineDimension,
      GridLine_FreshFoodLowHystDelta);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenFreezerShiftChanges)
{
   Given TheGridLineCalculationErdsAreInitialized();
   And TheModuleIsInitialized();

   And FreezerThermalShiftIs(AnotherFreezerShiftTemperature);

   And CalculatedGridLineTempShouldBe(
      (600 + AnotherFreezerShiftTemperature),
      GridDelta_Freezer,
      GridLine_FreezerExtraHigh);
}


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

   AFreshFoodAdjustedSetpointTemperature = 9,
   AnotherFreshFoodAdjustedSetpointTemperature = 9 * 2,

   AFreezerAdjustedSetpointTemperature = 18,
   AnotherFreezerAdjustedSetpointTemperature = 18 * 2,

   ACrossAmbientHysteresisAdjustmentInDegFx100 = 23,
};

static const GridLineAdjustmentErds_t freshFoodGridLineAdjustmentErds = {
   .offsetInDegFx100Erd = Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_FreshFood_AdjustedSetpointInDegFx100
};

static const GridLineAdjustmentErds_t freezerGridLineAdjustmentErds = {
   .offsetInDegFx100Erd = Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
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
   const PersonalityParametricData_t *parametric;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      parametric = PersonalityParametricData_Get(dataModel);

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
      WhenTheFreezerAdjustedSetpointIs(freezerAdjustedSetpoint);
      WhenTheFreshFoodAdjustedSetpointIs(freshFoodAdjustedSetpoint);
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

   void WhenTheCrossAmbientHysteresisAdjustmentChangesTo(TemperatureDegFx100_t adjustment)
   {
      DataModel_Write(
         dataModel,
         Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
         &adjustment);
   }

   void GivenTheGridLineCalculationErdsAreInitialized()
   {
      GivenTheAdjustedSetpointsAre(AFreshFoodAdjustedSetpointTemperature,
         AFreezerAdjustedSetpointTemperature);

      GivenTheOffsetsAre(AFreshFoodOffsetTemperature,
         AFreezerOffsetTemperature);
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

      temperature +=
         parametric->gridData->deltaGridLines->gridLines[dimension].gridLineData[gridLineIndex].gridLinesDegFx100;

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
         AFreshFoodOffsetTemperature,
         GridDelta_FreshFood,
         GridLine_Nfl);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FreshFood,
         GridLine_FreshFoodLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FreshFood,
         GridLine_FreshFoodSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FreshFood,
         GridLine_FreshFoodHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FreshFood,
         GridLine_FreshFoodExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FreshFood,
         GridLine_FreshFoodSuperHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerSuperHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerOffsetTemperature,
         GridDelta_Freezer,
         GridLine_FreezerExtremeHigh);
   }
   void TheFreezerGridLineTemperaturesShouldBeInitializedValues()
   {
      TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_Freezer,
         GridLine_FreezerSuperHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreezerOffsetTemperature,
         GridDelta_Freezer,
         GridLine_FreezerExtremeHigh);
   }

   void TheFreshFoodGridLineTemperaturesShouldBeAdjustedWithCrossAmbientHysteresis()
   {
      TheCalculatedGridLineTempShouldBe(
         AFreshFoodOffsetTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FreshFood,
         GridLine_Nfl);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FreshFood,
         GridLine_FreshFoodLowHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FreshFood,
         GridLine_FreshFoodSetpointDelta);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FreshFood,
         GridLine_FreshFoodHighHyst);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FreshFood,
         GridLine_FreshFoodExtraHigh);

      And TheCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
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
      AnotherFreshFoodOffsetTemperature,
      GridDelta_FreshFood,
      GridLine_Nfl);

   And TheCalculatedGridLineTempShouldBe(
      AnotherFreezerOffsetTemperature,
      GridDelta_Freezer,
      GridLine_FreezerExtremeHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenAdjustedSetpointChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheFreshFoodAdjustedSetpointIs(AnotherFreshFoodAdjustedSetpointTemperature);
   WhenTheFreezerAdjustedSetpointIs(AnotherFreezerAdjustedSetpointTemperature);

   TheCalculatedGridLineTempShouldBe(
      AnotherFreshFoodAdjustedSetpointTemperature,
      GridDelta_FreshFood,
      GridLine_FreshFoodLowHyst);

   TheCalculatedGridLineTempShouldBe(
      AnotherFreshFoodAdjustedSetpointTemperature,
      GridDelta_FreshFood,
      GridLine_FreshFoodExtraHigh);

   TheCalculatedGridLineTempShouldBe(
      AnotherFreshFoodAdjustedSetpointTemperature,
      GridDelta_FreshFood,
      GridLine_FreshFoodSuperHigh);

   TheCalculatedGridLineTempShouldBe(
      AnotherFreezerAdjustedSetpointTemperature,
      FreezerGridLineDimension,
      GridLine_FreezerLowHyst);

   TheCalculatedGridLineTempShouldBe(
      AnotherFreezerAdjustedSetpointTemperature,
      FreezerGridLineDimension,
      GridLine_FreezerSuperHigh);
}

TEST(GridLineCalculator, ShouldRecalculateFreshFoodGridLinesWhenCrossAmbientHysteresisAdjustmentChanges)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitialized();

   WhenTheCrossAmbientHysteresisAdjustmentChangesTo(ACrossAmbientHysteresisAdjustmentInDegFx100);

   TheFreshFoodGridLineTemperaturesShouldBeAdjustedWithCrossAmbientHysteresis();
   TheFreezerGridLineTemperaturesShouldBeInitializedValues();
}

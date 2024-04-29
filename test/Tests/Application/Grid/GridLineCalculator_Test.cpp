
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
   AnotherFreshFoodOffsetTemperature = AFreshFoodOffsetTemperature * 2,

   AFreezerOffsetTemperature = 5,
   AnotherFreezerOffsetTemperature = AFreezerOffsetTemperature * 2,

   AFeaturePanOffsetTemperature = 15,
   AnotherFeaturePanOffsetTemperature = AFeaturePanOffsetTemperature * 2,

   AFreshFoodAdjustedSetpointTemperature = 9,
   AnotherFreshFoodAdjustedSetpointTemperature = AFreshFoodAdjustedSetpointTemperature * 2,

   AFreezerAdjustedSetpointTemperature = 18,
   AnotherFreezerAdjustedSetpointTemperature = AFreezerAdjustedSetpointTemperature * 2,

   AFeaturePanAdjustedSetpointTemperature = 27,
   AnotherFeaturePanAdjustedSetpointTemperature = AFeaturePanAdjustedSetpointTemperature * 2,

   ACrossAmbientHysteresisAdjustmentInDegFx100 = 23,
};

static const GridLineAdjustmentErds_t freshFoodAndFreezerFirstDimensionGridLinesAdjustmentErds = {
   .offsetInDegFx100Erd = Erd_FreshFood_CabinetPlusCrossAmbientOffsetInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_FreshFood_AdjustedSetpointInDegFx100
};

static const GridLineAdjustmentErds_t freshFoodAndFreezerSecondDimensionGridLinesAdjustmentErds = {
   .offsetInDegFx100Erd = Erd_Freezer_CabinetPlusCrossAmbientOffsetInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_Freezer_AdjustedSetpointInDegFx100,
};

static const GridLineAdjustmentErds_t featurePanFirstDimensionGridLinesAdjustmentErds = {
   .offsetInDegFx100Erd = Erd_FeaturePan_CrossAmbientOffsetInDegFx100,
   .adjustedSetpointInDegFx100Erd = Erd_FeaturePan_AdjustedSetpointInDegFx100
};

static const GridLineCalculatorConfiguration_t freshFoodAndFreezerConfig = {
   .calculatedGridLineErd = Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .gridDeltaOffsetErd = Erd_U8Zero,
   .gridLineAdjustmentErds = {
      freshFoodAndFreezerFirstDimensionGridLinesAdjustmentErds,
      freshFoodAndFreezerSecondDimensionGridLinesAdjustmentErds,
   }
};

static const GridLineCalculatorConfiguration_t featurePanConfig = {
   .calculatedGridLineErd = Erd_FeaturePanGrid_CalculatedGridLines,
   .crossAmbientHysteresisAdjustmentErd = Erd_CrossAmbientHysteresisAdjustmentInDegFx100,
   .gridDeltaOffsetErd = Erd_FeaturePanCurrentMode,
   .gridLineAdjustmentErds = {
      featurePanFirstDimensionGridLinesAdjustmentErds,
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

   void GivenTheModuleIsInitializedForTwoDimensionalGrid()
   {
      GridLineCalculator_Init(
         &instance,
         &freshFoodAndFreezerConfig,
         parametric->freshFoodAndFreezerGridData,
         dataModel);
   }

   void GivenTheModuleIsInitializedForOneDimensionalGrid()
   {
      GridLineCalculator_Init(
         &instance,
         &featurePanConfig,
         parametric->featurePanGridData,
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

   void WhenTheFeaturePanAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_FeaturePan_AdjustedSetpointInDegFx100,
         &setpoint);
   }

   void WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_t mode)
   {
      DataModel_Write(
         dataModel,
         Erd_FeaturePanCurrentMode,
         &mode);
   }

   void GivenTheAdjustedSetpointsAre(
      TemperatureDegFx100_t freshFoodAdjustedSetpoint,
      TemperatureDegFx100_t freezerAdjustedSetpoint,
      TemperatureDegFx100_t featurePanAdjustedSetpoint)
   {
      WhenTheFreezerAdjustedSetpointIs(freezerAdjustedSetpoint);
      WhenTheFreshFoodAdjustedSetpointIs(freshFoodAdjustedSetpoint);
      WhenTheFeaturePanAdjustedSetpointIs(featurePanAdjustedSetpoint);
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

   void WhenTheFeaturePanCrossAmbientOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_FeaturePan_CrossAmbientOffsetInDegFx100,
         &offset);
   }

   void GivenTheOffsetsAre(
      TemperatureDegFx100_t freshFoodOffset,
      TemperatureDegFx100_t freezerOffset,
      TemperatureDegFx100_t featurePanOffset)
   {
      WhenTheFreshFoodSumOffsetIs(freshFoodOffset);
      WhenTheFreezerSumOffsetIs(freezerOffset);
      WhenTheFeaturePanCrossAmbientOffsetIs(featurePanOffset);
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
      WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_Off);

      GivenTheAdjustedSetpointsAre(
         AFreshFoodAdjustedSetpointTemperature,
         AFreezerAdjustedSetpointTemperature,
         AFeaturePanAdjustedSetpointTemperature);

      GivenTheOffsetsAre(
         AFreshFoodOffsetTemperature,
         AFreezerOffsetTemperature,
         AFeaturePanOffsetTemperature);
   }

   void GivenTheAdjustedSetpointPluginReadyIs(bool state)
   {
      DataModel_Write(dataModel, Erd_AdjustedSetpointPluginReady, &state);
   }

   void TheOneDimensionalCalculatedGridLineTempShouldBe(TemperatureDegFx100_t temperature, uint8_t gridLineIndex)
   {
      TwoDimensionalCalculatedGridLines_t calcLines;
      DataModel_Read(dataModel, Erd_FeaturePanGrid_CalculatedGridLines, &calcLines);

      FeaturePanCurrentMode_t gridDeltaOffset;
      DataModel_Read(dataModel, Erd_FeaturePanCurrentMode, &gridDeltaOffset);

      gridDeltaOffset = TRUNCATE_UNSIGNED_SUBTRACTION(gridDeltaOffset, 1);

      temperature +=
         parametric->featurePanGridData->deltaGridLines->gridLines[GridDelta_FirstDimension].gridLineData[(CalculatedGridLines_MaxGridLinesCount * gridDeltaOffset) + gridLineIndex].gridLinesDegFx100;

      CHECK_EQUAL(temperature, calcLines.firstDimensionGridLines.gridLinesDegFx100[gridLineIndex]);
   }

   void TheTwoDimensionalCalculatedGridLineTempShouldBe(
      TemperatureDegFx100_t temperature,
      uint8_t dimension,
      uint8_t gridLineIndex)
   {
      TwoDimensionalCalculatedGridLines_t calcLines;
      DataModel_Read(dataModel,
         Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
         &calcLines);

      temperature +=
         parametric->freshFoodAndFreezerGridData->deltaGridLines->gridLines[dimension].gridLineData[gridLineIndex].gridLinesDegFx100;

      if(dimension == GridDelta_FirstDimension)
      {
         CHECK_EQUAL(temperature,
            calcLines.firstDimensionGridLines.gridLinesDegFx100[gridLineIndex]);
      }
      else
      {
         CHECK_EQUAL(temperature,
            calcLines.secondDimensionGridLines.gridLinesDegFx100[gridLineIndex]);
      }
   }

   void TheTwoDimensionalGridLineTemperaturesShouldBeInitializedValues()
   {
      TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodOffsetTemperature,
         GridDelta_FirstDimension,
         GridLine_Nfl);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FirstDimension,
         GridLine_FreshFoodLowHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FirstDimension,
         GridLine_FreshFoodSetpointDelta);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FirstDimension,
         GridLine_FreshFoodHighHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FirstDimension,
         GridLine_FreshFoodExtraHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature,
         GridDelta_FirstDimension,
         GridLine_FreshFoodSuperHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerLowHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerSetpointDelta);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerHighHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerExtraHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerSuperHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerOffsetTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerExtremeHigh);
   }

   void TheOneDimensionalGridLineTemperaturesShouldBeInitializedValues()
   {
      TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_1);
      And TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_2);
      And TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_3);
      And TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_4);
      And TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_5);
      And TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_6);
      And TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_7);
   }

   void TheFreezerGridLineTemperaturesShouldBeInitializedValues()
   {
      TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerLowHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerSetpointDelta);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerHighHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerExtraHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerAdjustedSetpointTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerSuperHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreezerOffsetTemperature,
         GridDelta_SecondDimension,
         GridLine_FreezerExtremeHigh);
   }

   void TheFreshFoodGridLineTemperaturesShouldBeAdjustedWithCrossAmbientHysteresis()
   {
      TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodOffsetTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FirstDimension,
         GridLine_Nfl);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FirstDimension,
         GridLine_FreshFoodLowHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature + ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FirstDimension,
         GridLine_FreshFoodSetpointDelta);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature - ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FirstDimension,
         GridLine_FreshFoodHighHyst);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature - ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FirstDimension,
         GridLine_FreshFoodExtraHigh);

      And TheTwoDimensionalCalculatedGridLineTempShouldBe(
         AFreshFoodAdjustedSetpointTemperature - ACrossAmbientHysteresisAdjustmentInDegFx100,
         GridDelta_FirstDimension,
         GridLine_FreshFoodSuperHigh);
   };
};

TEST(GridLineCalculator, ShouldCalculateGridLinesOnInitIncludingAdjustmentErds_TwoDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForTwoDimensionalGrid();
   TheTwoDimensionalGridLineTemperaturesShouldBeInitializedValues();
}

TEST(GridLineCalculator, ShouldAssertWhenAdjustedSetpointPluginIsNotReadyOnInit_TwoDimensionalGrid)
{
   GivenTheAdjustedSetpointPluginReadyIs(false);
   ShouldFailAssertion(GivenTheModuleIsInitializedForTwoDimensionalGrid());
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenOffsetChanges_TwoDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForTwoDimensionalGrid();

   WhenTheFreshFoodSumOffsetIs(AnotherFreshFoodOffsetTemperature);
   WhenTheFreezerSumOffsetIs(AnotherFreezerOffsetTemperature);

   TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreshFoodOffsetTemperature,
      GridDelta_FirstDimension,
      GridLine_Nfl);

   And TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreezerOffsetTemperature,
      GridDelta_SecondDimension,
      GridLine_FreezerExtremeHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenAdjustedSetpointChanges_TwoDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForTwoDimensionalGrid();

   WhenTheFreshFoodAdjustedSetpointIs(AnotherFreshFoodAdjustedSetpointTemperature);
   WhenTheFreezerAdjustedSetpointIs(AnotherFreezerAdjustedSetpointTemperature);

   TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreshFoodAdjustedSetpointTemperature,
      GridDelta_FirstDimension,
      GridLine_FreshFoodLowHyst);

   TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreshFoodAdjustedSetpointTemperature,
      GridDelta_FirstDimension,
      GridLine_FreshFoodExtraHigh);

   TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreshFoodAdjustedSetpointTemperature,
      GridDelta_FirstDimension,
      GridLine_FreshFoodSuperHigh);

   TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreezerAdjustedSetpointTemperature,
      GridDelta_SecondDimension,
      GridLine_FreezerLowHyst);

   TheTwoDimensionalCalculatedGridLineTempShouldBe(
      AnotherFreezerAdjustedSetpointTemperature,
      GridDelta_SecondDimension,
      GridLine_FreezerSuperHigh);
}

TEST(GridLineCalculator, ShouldRecalculateFreshFoodGridLinesWhenCrossAmbientHysteresisAdjustmentChanges_TwoDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForTwoDimensionalGrid();

   WhenTheCrossAmbientHysteresisAdjustmentChangesTo(ACrossAmbientHysteresisAdjustmentInDegFx100);

   TheFreshFoodGridLineTemperaturesShouldBeAdjustedWithCrossAmbientHysteresis();
   TheFreezerGridLineTemperaturesShouldBeInitializedValues();
}

TEST(GridLineCalculator, ShouldCalculateGridLinesOnInitIncludingAdjustmentErds_OneDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForOneDimensionalGrid();
   TheOneDimensionalGridLineTemperaturesShouldBeInitializedValues();
}

TEST(GridLineCalculator, ShouldAssertWhenAdjustedSetpointPluginIsNotReadyOnInit_OneDimensionalGrid)
{
   GivenTheAdjustedSetpointPluginReadyIs(false);
   ShouldFailAssertion(GivenTheModuleIsInitializedForOneDimensionalGrid());
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenAdjustedSetpointChanges_OneDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForOneDimensionalGrid();

   WhenTheFeaturePanAdjustedSetpointIs(AnotherFeaturePanAdjustedSetpointTemperature);

   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_6);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AnotherFeaturePanAdjustedSetpointTemperature, GridLine_7);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenGridOffset_OneDimensionalGrid)
{
   GivenTheGridLineCalculationErdsAreInitialized();
   GivenTheModuleIsInitializedForOneDimensionalGrid();

   WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_Mode1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_6);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_7);

   WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_Mode2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_6);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_7);

   WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_Mode3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_6);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_7);

   WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_Mode4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_6);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_7);

   WhenTheFeaturePanCurrentModeIs(FeaturePanCurrentMode_Mode5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_1);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_2);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_3);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_4);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_5);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_6);
   TheOneDimensionalCalculatedGridLineTempShouldBe(AFeaturePanAdjustedSetpointTemperature, GridLine_7);
}

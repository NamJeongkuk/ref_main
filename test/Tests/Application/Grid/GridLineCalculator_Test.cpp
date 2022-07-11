
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
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define And
#define Then
#define The
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

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = DeltaGridLines_BitMapping_OffsetBitMask,
   },
   {
      .gridLinesDegFx100 = -450,
      .bitMapping = DeltaGridLines_BitMapping_AdjSetpointBitMask,
   },
   {
      .gridLinesDegFx100 = 150,
      .bitMapping = DeltaGridLines_BitMapping_ShiftBitMask,
   },
   {
      .gridLinesDegFx100 = 450,
      .bitMapping = DeltaGridLines_BitMapping_SetpointBitMask,
   },
   {
      .gridLinesDegFx100 = 950,
      .bitMapping = DeltaGridLines_BitMapping_AdjSetpointBitMask,
   },
   {
      .gridLinesDegFx100 = 1150,
      .bitMapping = DeltaGridLines_BitMapping_AdjSetpointBitMask,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .bitMapping = DeltaGridLines_BitMapping_AdjSetpointBitMask,
   },
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = DeltaGridLines_BitMapping_OffsetBitMask,
   },
   {
      .gridLinesDegFx100 = 250,
      .bitMapping = DeltaGridLines_BitMapping_SetpointBitMask,
   },
   {
      .gridLinesDegFx100 = 600,
      .bitMapping = DeltaGridLines_BitMapping_ShiftBitMask,
   },
   {
      .gridLinesDegFx100 = 750,
      .bitMapping = DeltaGridLines_BitMapping_AdjSetpointBitMask |
         DeltaGridLines_BitMapping_OffsetBitMask,
   },
   {
      .gridLinesDegFx100 = 5500,
      .bitMapping = DeltaGridLines_BitMapping_OffsetBitMask,
   },
};

static const DeltaAxisGridLines_t freshFoodAxis = {
   .numberOfLines = NumberOfGridLinesPerAxis,
   .gridLineData = freshFoodGridLineData,
};

static const DeltaAxisGridLines_t freezerAxis = {
   .numberOfLines = NumberOfGridLinesPerAxis,
   .gridLineData = freezerGridLineData
};

static DeltaAxisGridLines_t parametricGrid[] = {
   freshFoodAxis,
   freezerAxis
};

static DeltaGridLines_t deltaGrid = {
   .dimensions = NumberGridDimensions,
   .gridLines = parametricGrid
};

static const GridData_t gridData = {
   .gridId = 0,
   .deltaGridLines = &deltaGrid,
   .gridPeriodicRunRateInMSec = MSEC_PER_SEC
};
static const GridLineAdjustmentErds_t freshFoodGridLineAdjustmentErds{
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .offsetErd = Erd_FreshFood_Offset,
   .shiftErd = Erd_FreshFood_Shift,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpoint
};

static const GridLineAdjustmentErds_t freezerGridLineAdjustmentErds{
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .offsetErd = Erd_Freezer_Offset,
   .shiftErd = Erd_Freezer_Shift,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpoint,
};

static const GridLineCalculatorConfiguration_t config{
   .calculatedGridLineErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTempErd = Erd_FreshFood_FilteredTemperatureResolved,
   .freezerFilteredTemperatureErd = Erd_Freezer_FilteredTemperatureResolved,
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
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void ModuleIsInitialized()
   {
      GridLineCalculator_Init(
         &instance,
         &config,
         dataModel);
   }

   void FreshFoodAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_AdjustedSetpoint,
         &setpoint);
   }

   void FreezerAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_AdjustedSetpoint,
         &setpoint);
   }

   void AdjustedSetpointsAre(
      TemperatureDegFx100_t freshFoodAdjustedSetpoint,
      TemperatureDegFx100_t freezerAdjustedSetpoint)
   {
      FreshFoodAdjustedSetpointIs(freshFoodAdjustedSetpoint);
      FreezerAdjustedSetpointIs(freezerAdjustedSetpoint);
   }

   void FreshFoodOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_Offset,
         &offset);
   }

   void FreezerOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_Offset,
         &offset);
   }

   void OffsetsAre(
      TemperatureDegFx100_t freshFoodOffset,
      TemperatureDegFx100_t freezerOffset)
   {
      FreshFoodOffsetIs(freshFoodOffset);
      FreezerOffsetIs(freezerOffset);
   }

   void FreezerShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_Freezer_Shift,
         &shift);
   }

   void FreshFoodShiftIs(TemperatureDegFx100_t shift)
   {
      DataModel_Write(dataModel,
         Erd_FreshFood_Shift,
         &shift);
   }

   void ShiftsAre(
      TemperatureDegFx100_t freshFoodShift,
      TemperatureDegFx100_t freezerShift)
   {
      FreshFoodShiftIs(freshFoodShift);
      FreezerShiftIs(freezerShift);
   }

   void FreshFoodRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperature = rawSetpoint,
         .care = true
      };

      DataModel_Write(dataModel,
         Erd_FreshFoodSetpoint_ResolvedVote,
         &temp);
   }

   void FreezerRawSetpointIs(TemperatureDegFx100_t rawSetpoint)
   {
      SetpointVotedTemperature_t temp = {
         .temperature = rawSetpoint,
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

   void GridLineCalculationErdsAreInitialized()
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

   void CalculatedGridLineTempShouldBe(
      TemperatureDegFx100_t temperature,
      uint8_t dimension,
      uint8_t gridLineIndex)
   {
      CalculatedGridLines_t calcLines;
      DataModel_Read(dataModel,
         Erd_Grid_CalculatedGridLines,
         &calcLines);

      CHECK_EQUAL(temperature,
         calcLines.gridLines[dimension].gridLinesDegFx100[gridLineIndex]);
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
         (750 + AFreezerAdjustedSetpointTemperature + AFreezerOffsetTemperature),
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
   Given The ModuleIsInitialized();
}

TEST(GridLineCalculator, ShouldCalculateGridLinesOnInitIncludingAdjustmentErds)
{
   Given The GridLineCalculationErdsAreInitialized();
   And The ModuleIsInitialized();
   GridLineTemperaturesShouldBeInitializedValues();
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenOffsetChanges)
{
   Given The GridLineCalculationErdsAreInitialized();
   And The ModuleIsInitialized();

   When FreshFoodOffsetIs(AnotherFreshFoodOffsetTemperature);
   And FreezerOffsetIs(AnotherFreezerOffsetTemperature);

   CalculatedGridLineTempShouldBe(
      (0 + AnotherFreshFoodOffsetTemperature),
      FreshFoodGridLineDimension,
      GridLine_Nfl);

   And CalculatedGridLineTempShouldBe((0 + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerDelta);

   And CalculatedGridLineTempShouldBe(
      (750 + AFreezerAdjustedSetpointTemperature + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerSuperHigh);

   And CalculatedGridLineTempShouldBe(
      (5500 + AnotherFreezerOffsetTemperature),
      GridDelta_Freezer,
      GridLine_FreezerExtremeHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenShiftChanges)
{
   Given The GridLineCalculationErdsAreInitialized();
   And The ModuleIsInitialized();

   When FreshFoodShiftIs(AnotherFreshFoodShiftTemperature);
   And FreezerShiftIs(AnotherFreezerShiftTemperature);

   CalculatedGridLineTempShouldBe((150 + AnotherFreshFoodShiftTemperature), FreshFoodGridLineDimension, GridLine_FreshFoodLowHystDelta);
   And CalculatedGridLineTempShouldBe((600 + AnotherFreezerShiftTemperature), GridDelta_Freezer, GridLine_FreezerExtraHigh);
}

TEST(GridLineCalculator, ShouldRecalculateGridLinesWhenSetpointChanges)
{
   Given The GridLineCalculationErdsAreInitialized();
   And The ModuleIsInitialized();

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
   Given The GridLineCalculationErdsAreInitialized();
   And The ModuleIsInitialized();

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
      (750 + AnotherFreezerAdjustedSetpointTemperature + AFreezerOffsetTemperature),
      FreezerGridLineDimension,
      GridLine_FreezerSuperHigh);
}

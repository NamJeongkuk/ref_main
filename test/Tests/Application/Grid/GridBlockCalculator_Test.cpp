/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "GridBlockCalculator.h"
#include "GridLineCalculator.h"
#include "utils.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define And
#define Then
#define The
#define When
#define Given

#define EXPECTED_PREVIOUS_VALUES(_count, _one, _two, _three, _four, _five) \
   expectedPreviousGridBlockNumbers = {                                    \
      .count = _count,                                                     \
      .blockNumbers = { _one, _two, _three, _four, _five }                 \
   };                                                                      \
   The PreviousGridBlocksShouldBe(&expectedPreviousGridBlockNumbers);

enum
{
   AVeryHighTemp = 5600,
   AVeryLowTemp = -750,
   FreshFoodFallbackTemp = -500,
   FreezerFallbackTemp = 2500
};

enum
{
   Invalid = false,
   Valid = true
};

static const CrossAmbientHysteresisAdjustmentData_t disabledCrossAmbientHysteresisAdjustment = {
   .multiplier = 0,
   .divider = 1
};

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = -450,
      .correction = DeltaGridLinesCorrection_Offset,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 0,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 150,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 450,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 950,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 1150,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 0,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 250,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 600,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 750,
      .correction = DeltaGridLinesCorrection_AdjustedSetpoint,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
   },
   {
      .gridLinesDegFx100 = 5500,
      .correction = DeltaGridLinesCorrection_Offset,
      .crossAmbientAdjustment = &disabledCrossAmbientHysteresisAdjustment,
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

static CalculatedAxisGridLines_t freshFoodCalculatedAxis = {
   .gridLinesDegFx100 = { 0, 150, 250, 350, 450, 5500 }
};

static CalculatedAxisGridLines_t freezerCalculatedAxis = {
   .gridLinesDegFx100 = { -250, 0, 150, 250, 350, 2150 }
};

static CalculatedAxisGridLines_t differentFreshFoodCalculatedAxis = {
   .gridLinesDegFx100 = { 150, 250, 350, 450, 550, 5500 }
};

static CalculatedAxisGridLines_t differentFreezerCalculatedAxis = {
   .gridLinesDegFx100 = { 0, 150, 250, 350, 450, 2150 }
};

static CalculatedGridLines_t calculatedGridLines = {
   .freshFoodGridLine = freshFoodCalculatedAxis,
   .freezerGridLine = freezerCalculatedAxis
};

static CalculatedGridLines_t differentCalculatedGridLines = {
   .freshFoodGridLine = differentFreshFoodCalculatedAxis,
   .freezerGridLine = differentFreezerCalculatedAxis
};

static const GridBlockCalculatorConfiguration_t config = {
   .freshFoodFilteredResolvedTemperatureInDegFx100 = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerFilteredResolvedTemperatureInDegFx100 = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .currentGridBlockNumberErd = Erd_Grid_BlockNumber,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .previousGridBlockNumbersErd = Erd_Grid_PreviousBlocks,
   .freezerThermistorIsValidErd = Erd_FreezerThermistor_IsValidResolved,
   .freshFoodThermistorIsValidErd = Erd_FreshFoodThermistor_IsValidResolved
};

TEST_GROUP(GridBlockCalculator)
{
   GridBlockCalculator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(
         dataModel,
         Erd_TimerModule,
         &timerModuleTestDouble->timerModule);
   }

   void ModuleIsInitialized(void)
   {
      GridBlockCalculator_Init(
         &instance,
         &config,
         dataModel,
         PersonalityParametricData_Get(dataModel)->gridData);
   }

   void GridLinesAreReady()
   {
      DataModel_Write(dataModel, Erd_GridLineCalculatorIsReady, set);
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      GridLinesAreReady();
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void FreshFoodFilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerFilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void CalculatedGridBlockIs(GridBlockNumber_t gridBlockNumber)
   {
      DataModel_Write(dataModel, Erd_Grid_BlockNumber, &gridBlockNumber);
   }

   void CurrentGridBlockShouldBe(GridBlockNumber_t expected)
   {
      GridBlockNumber_t actual;

      DataModel_Read(dataModel, Erd_Grid_BlockNumber, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void PreviousGridBlockCountShouldBe(uint8_t expected)
   {
      PreviousGridBlockNumbers_t previousGridBlockNumbers;
      DataModel_Read(
         dataModel,
         Erd_Grid_PreviousBlocks,
         &previousGridBlockNumbers);

      CHECK_EQUAL(expected, previousGridBlockNumbers.count);
   }

   void PreviousGridBlocksShouldBe(GridBlockNumber_t * expected)
   {
      PreviousGridBlockNumbers_t previousGridBlockNumbers;

      DataModel_Read(
         dataModel,
         Erd_Grid_PreviousBlocks,
         &previousGridBlockNumbers);

      MEMCMP_EQUAL(expected, previousGridBlockNumbers.blockNumbers, PreviousGridBlockNumbers_MaxBlockNumbers);
   }

   void AddBlockToExpectedArray(GridBlockNumber_t * expected, GridBlockNumber_t currentBlockNumber, uint8_t currentBlockCount)
   {
      if(currentBlockCount >= NumberOfPreviousGridBlocksStored)
      {
         uint8_t i;
         for(i = NumberOfPreviousGridBlocksStored - 1; i > 0; i--)
         {
            expected[i] = expected[i - 1];
         }
         expected[i] = currentBlockNumber;
      }
      else
      {
         expected[(NumberOfPreviousGridBlocksStored - 1) - currentBlockCount] = currentBlockNumber;
      }
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void BothThermistorsAreValid(void)
   {
      FreezerThermistorValidityIs(Valid);
      FreshFoodThermistorValidityIs(Valid);
   }
};

TEST(GridBlockCalculator, ShouldInitialize)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And The ModuleIsInitialized();
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperatures)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreshFoodFilteredTemperatureIs(349);
   And FreezerFilteredTemperatureIs(149);
   And CurrentGridBlockShouldBe(31);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   CurrentGridBlockShouldBe(42);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   And FreezerFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockShouldBe(0);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockShouldBe(6);

   When FreshFoodFilteredTemperatureIs(350);
   And FreezerFilteredTemperatureIs(150);
   CurrentGridBlockShouldBe(31);

   When FreshFoodFilteredTemperatureIs(351);
   And FreezerFilteredTemperatureIs(150);
   CurrentGridBlockShouldBe(32);

   When FreshFoodFilteredTemperatureIs(351);
   And FreezerFilteredTemperatureIs(151);
   CurrentGridBlockShouldBe(25);
}

TEST(GridBlockCalculator, ShouldCalculatePreviousGridBlocks)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   The CurrentGridBlockShouldBe(48);
   GridBlockNumber_t expectedPreviousBlocks[NumberOfPreviousGridBlocksStored] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
   uint8_t expectedBlockIndex = 0;

   PreviousGridBlockCountShouldBe(0);
   PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 48, expectedBlockIndex++);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   CurrentGridBlockShouldBe(42);
   And PreviousGridBlockCountShouldBe(1);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 42, expectedBlockIndex++);

   When FreezerFilteredTemperatureIs(AVeryHighTemp);
   The CurrentGridBlockShouldBe(0);
   And PreviousGridBlockCountShouldBe(2);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 0, expectedBlockIndex++);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockShouldBe(6);
   And The PreviousGridBlockCountShouldBe(3);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 6, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(AVeryLowTemp);
   The CurrentGridBlockShouldBe(48);
   And PreviousGridBlockCountShouldBe(4);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 48, expectedBlockIndex++);

   When FreezerFilteredTemperatureIs(350);
   The CurrentGridBlockShouldBe(20);
   And PreviousGridBlockCountShouldBe(5);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 20, expectedBlockIndex++);

   When FreshFoodFilteredTemperatureIs(0);
   The CurrentGridBlockShouldBe(14);
   And PreviousGridBlockCountShouldBe(5);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 14, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(4500);
   The CurrentGridBlockShouldBe(0);
   And PreviousGridBlockCountShouldBe(5);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);
}

TEST(GridBlockCalculator, ShouldRecalculateBlocksAndAddToPreviousBlocksWhenCalculatedGridLinesChange)
{
   GridBlockNumber_t expectedPreviousBlocks[NumberOfPreviousGridBlocksStored] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
   uint8_t expectedBlockIndex = 0;

   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(275);
   And FreezerFilteredTemperatureIs(0);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(38);
   And PreviousGridBlockCountShouldBe(0);
   And PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 38, expectedBlockIndex++);

   When CalculatedGridLinesAre(differentCalculatedGridLines);
   CurrentGridBlockShouldBe(44);
   The PreviousGridBlockCountShouldBe(1);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);
}

TEST(GridBlockCalculator, ShouldNotUpdatePreviousGridBlocksIfGridBlockDoesntChange)
{
   uint8_t expectedBlockIndex = 0;
   GridBlockNumber_t expectedPreviousBlocks[NumberOfPreviousGridBlocksStored] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(275);
   And FreezerFilteredTemperatureIs(0);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(38);
   The PreviousGridBlockCountShouldBe(0);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   When FreshFoodFilteredTemperatureIs(276);
   CurrentGridBlockShouldBe(38);
   The PreviousGridBlockCountShouldBe(0);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   When FreshFoodFilteredTemperatureIs(277);
   CurrentGridBlockShouldBe(38);
   The PreviousGridBlockCountShouldBe(0);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 38, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(1);
   CurrentGridBlockShouldBe(31);
   The PreviousGridBlockCountShouldBe(1);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(2);
   CurrentGridBlockShouldBe(31);
   The PreviousGridBlockCountShouldBe(1);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(3);
   CurrentGridBlockShouldBe(31);
   The PreviousGridBlockCountShouldBe(1);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 31, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(5900);
   CurrentGridBlockShouldBe(3);
   The PreviousGridBlockCountShouldBe(2);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(5901);
   CurrentGridBlockShouldBe(3);
   The PreviousGridBlockCountShouldBe(2);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(5902);
   CurrentGridBlockShouldBe(3);
   The PreviousGridBlockCountShouldBe(2);
   And The PreviousGridBlocksShouldBe(expectedPreviousBlocks);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperaturesWithAnInvalidFreshFoodThermistor)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And FreezerThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Invalid);
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(43);

   When FreshFoodFilteredTemperatureIs(349);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(29);

   When FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockShouldBe(15);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockShouldBe(1);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperaturesWithAnInvalidFreezerThermistor)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And FreezerThermistorValidityIs(Invalid);
   And FreshFoodThermistorValidityIs(Valid);
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(27);

   When FreshFoodFilteredTemperatureIs(349);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(24);

   When FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockShouldBe(22);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockShouldBe(21);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreezerThermistorChangesFromValidToInvalidAndBackOnTemperatureChange)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockShouldBe(22);

   When FreezerThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockShouldBe(15);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockShouldBe(0);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreezerThermistorChangesFromValidToInvalidAndBackWithoutTemperatureChange)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockShouldBe(22);

   When FreezerThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockShouldBe(1);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockShouldBe(0);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreshFoodThermistorChangesFromValidToInvalidAndBackOnTemperatureChange)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockShouldBe(1);

   When FreshFoodThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockShouldBe(15);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockShouldBe(0);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreshFoodThermistorChangesFromValidToInvalidAndBackWithoutTemperatureChange)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(29);

   When FreshFoodThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(34);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockShouldBe(0);
}

TEST(GridBlockCalculator, ShouldNotCalculateNewGridBlockWhenBothThermistorsChangesFromValidToInvalid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(29);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(29);
}

TEST(GridBlockCalculator, ShouldRecalculateNewGridBlockWhenOneThermistorChangesBackToValidFromInvalid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitialized();

   CurrentGridBlockShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(29);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockShouldBe(29);

   When FreshFoodThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   The CurrentGridBlockShouldBe(27);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   The CurrentGridBlockShouldBe(48);
}

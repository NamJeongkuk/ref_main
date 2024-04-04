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
   The PreviousGridBlocksForTwoDimensionalGridShouldBe(&expectedPreviousGridBlockNumbers);

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

static const CalculatedAxisGridLines_t freshFoodCalculatedAxis = {
   .gridLinesDegFx100 = { 0, 150, 250, 350, 450, 5500 }
};

static const CalculatedAxisGridLines_t freezerCalculatedAxis = {
   .gridLinesDegFx100 = { -250, 0, 150, 250, 350, 2150 }
};

static const CalculatedAxisGridLines_t differentFreshFoodCalculatedAxis = {
   .gridLinesDegFx100 = { 150, 250, 350, 450, 550, 5500 }
};

static const CalculatedAxisGridLines_t differentFreezerCalculatedAxis = {
   .gridLinesDegFx100 = { 0, 150, 250, 350, 450, 2150 }
};

static CalculatedAxisGridLines_t oneDimensionalCalculatedAxis = {
   .gridLinesDegFx100 = { 0, 150, 250, 350, 450, 1000, 5500 }
};

static TwoDimensionalCalculatedGridLines_t calculatedGridLines = {
   .firstDimensionGridLines = freshFoodCalculatedAxis,
   .secondDimensionGridLines = freezerCalculatedAxis,
   .numberOfGridLinesPerDimension = NUM_ELEMENTS(freshFoodCalculatedAxis.gridLinesDegFx100)
};

static TwoDimensionalCalculatedGridLines_t differentCalculatedGridLines = {
   .firstDimensionGridLines = differentFreshFoodCalculatedAxis,
   .secondDimensionGridLines = differentFreezerCalculatedAxis,
   .numberOfGridLinesPerDimension = NUM_ELEMENTS(differentFreshFoodCalculatedAxis.gridLinesDegFx100)
};

static OneDimensionalCalculatedGridLines_t oneDimensionalCalculatedGridLines = {
   .gridLines = oneDimensionalCalculatedAxis,
   .numberOfGridLines = NUM_ELEMENTS(oneDimensionalCalculatedAxis.gridLinesDegFx100)
};

static const GridBlockAdjustmentErds_t firstDimensionGridBlockAdjustmentErds = {
   .filteredResolvedTemperatureInDegFx100 = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .thermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved
};

static const GridBlockAdjustmentErds_t secondDimensionGridBlockAdjustmentErds = {
   .filteredResolvedTemperatureInDegFx100 = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .thermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved
};

static const GridBlockCalculatorConfiguration_t twoDimensionalConfig = {
   .calculatedGridLinesErd = Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,
   .currentGridBlockNumberErd = Erd_FreshFoodAndFreezerGrid_BlockNumber,
   .previousGridBlockNumbersErd = Erd_FreshFoodAndFreezerGrid_PreviousBlocks,
   .gridBlockAdjustmentErds = {
      firstDimensionGridBlockAdjustmentErds,
      secondDimensionGridBlockAdjustmentErds }
};

static const GridBlockCalculatorConfiguration_t oneDimensionalConfig = {
   .calculatedGridLinesErd = Erd_FeaturePanGrid_CalculatedGridLines,
   .currentGridBlockNumberErd = Erd_FeaturePanGrid_BlockNumber,
   .previousGridBlockNumbersErd = Erd_FeaturePanGrid_PreviousBlocks,
   .gridBlockAdjustmentErds = { firstDimensionGridBlockAdjustmentErds }
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

   void ModuleIsInitializedWithTwoDimensionalGrid(void)
   {
      GridBlockCalculator_Init(
         &instance,
         &twoDimensionalConfig,
         dataModel,
         PersonalityParametricData_Get(dataModel)->freshFoodAndFreezerGridData);
   }

   void ModuleIsInitializedWithOneDimensionalGrid(void)
   {
      GridBlockCalculator_Init(
         &instance,
         &oneDimensionalConfig,
         dataModel,
         PersonalityParametricData_Get(dataModel)->featurePanGridData);
   }

   void GridLinesAreReady()
   {
      DataModel_Write(dataModel, Erd_GridLineCalculatorIsReady, set);
   }

   void CalculatedGridLinesAre(TwoDimensionalCalculatedGridLines_t gridLines)
   {
      GridLinesAreReady();
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &gridLines);
   }

   void OneDimensionalCalculatedGridLinesAre(OneDimensionalCalculatedGridLines_t gridLines)
   {
      GridLinesAreReady();
      DataModel_Write(dataModel, Erd_FeaturePanGrid_CalculatedGridLines, &gridLines);
   }

   void FreshFoodFilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void FreezerFilteredTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void CurrentGridBlockForTwoDimensionalGridShouldBe(GridBlockNumber_t expected)
   {
      GridBlockNumber_t actual;

      DataModel_Read(dataModel, Erd_FreshFoodAndFreezerGrid_BlockNumber, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CurrentGridBlockForOneDimensionalGridShouldBe(GridBlockNumber_t expected)
   {
      GridBlockNumber_t actual;

      DataModel_Read(dataModel, Erd_FeaturePanGrid_BlockNumber, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void PreviousGridBlockCountForTwoDimensionalGridShouldBe(uint8_t expected)
   {
      PreviousGridBlockNumbers_t previousGridBlockNumbers;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodAndFreezerGrid_PreviousBlocks,
         &previousGridBlockNumbers);

      CHECK_EQUAL(expected, previousGridBlockNumbers.count);
   }

   void PreviousGridBlocksForTwoDimensionalGridShouldBe(GridBlockNumber_t * expected)
   {
      PreviousGridBlockNumbers_t previousGridBlockNumbers;

      DataModel_Read(
         dataModel,
         Erd_FreshFoodAndFreezerGrid_PreviousBlocks,
         &previousGridBlockNumbers);

      MEMCMP_EQUAL(expected, previousGridBlockNumbers.blockNumbers, PreviousGridBlockNumbers_MaxBlockNumbers);
   }

   void AddBlockToExpectedArray(GridBlockNumber_t * expected, GridBlockNumber_t currentBlockNumber, uint8_t currentBlockCount)
   {
      if(currentBlockCount >= GridBlockCalculator_NumberOfPreviousGridBlocksToStore)
      {
         uint8_t i;
         for(i = GridBlockCalculator_NumberOfPreviousGridBlocksToStore - 1; i > 0; i--)
         {
            expected[i] = expected[i - 1];
         }
         expected[i] = currentBlockNumber;
      }
      else
      {
         expected[(GridBlockCalculator_NumberOfPreviousGridBlocksToStore - 1) - currentBlockCount] = currentBlockNumber;
      }
   }

   void FreshFoodThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void BothThermistorsAreValid(void)
   {
      FreshFoodThermistorValidityIs(Valid);
      FreezerThermistorValidityIs(Valid);
   }
};

TEST(GridBlockCalculator, ShouldInitializeWithAOneDimensionalGrid)
{
   Given OneDimensionalCalculatedGridLinesAre(oneDimensionalCalculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And The ModuleIsInitializedWithOneDimensionalGrid();
}

TEST(GridBlockCalculator, ShouldInitializeWithATwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And The ModuleIsInitializedWithTwoDimensionalGrid();
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperaturesWithAOneDimensionalGrid)
{
   Given OneDimensionalCalculatedGridLinesAre(oneDimensionalCalculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreshFoodThermistorValidityIs(Valid);
   And The ModuleIsInitializedWithOneDimensionalGrid();

   CurrentGridBlockForOneDimensionalGridShouldBe(0);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   CurrentGridBlockForOneDimensionalGridShouldBe(7);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockForOneDimensionalGridShouldBe(0);

   When FreshFoodFilteredTemperatureIs(350);
   CurrentGridBlockForOneDimensionalGridShouldBe(4);

   When FreshFoodFilteredTemperatureIs(351);
   CurrentGridBlockForOneDimensionalGridShouldBe(3);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperaturesWithATwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreshFoodFilteredTemperatureIs(349);
   And FreezerFilteredTemperatureIs(149);
   And CurrentGridBlockForTwoDimensionalGridShouldBe(31);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   CurrentGridBlockForTwoDimensionalGridShouldBe(42);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   And FreezerFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockForTwoDimensionalGridShouldBe(0);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockForTwoDimensionalGridShouldBe(6);

   When FreshFoodFilteredTemperatureIs(350);
   And FreezerFilteredTemperatureIs(150);
   CurrentGridBlockForTwoDimensionalGridShouldBe(31);

   When FreshFoodFilteredTemperatureIs(351);
   And FreezerFilteredTemperatureIs(150);
   CurrentGridBlockForTwoDimensionalGridShouldBe(32);

   When FreshFoodFilteredTemperatureIs(351);
   And FreezerFilteredTemperatureIs(151);
   CurrentGridBlockForTwoDimensionalGridShouldBe(25);
}

TEST(GridBlockCalculator, ShouldCalculatePreviousGridBlocks_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   The CurrentGridBlockForTwoDimensionalGridShouldBe(48);
   GridBlockNumber_t expectedPreviousBlocks[GridBlockCalculator_NumberOfPreviousGridBlocksToStore] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
   uint8_t expectedBlockIndex = 0;

   PreviousGridBlockCountForTwoDimensionalGridShouldBe(0);
   PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 48, expectedBlockIndex++);

   When FreshFoodFilteredTemperatureIs(AVeryLowTemp);
   CurrentGridBlockForTwoDimensionalGridShouldBe(42);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(1);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 42, expectedBlockIndex++);

   When FreezerFilteredTemperatureIs(AVeryHighTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(0);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(2);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 0, expectedBlockIndex++);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   CurrentGridBlockForTwoDimensionalGridShouldBe(6);
   And The PreviousGridBlockCountForTwoDimensionalGridShouldBe(3);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 6, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(AVeryLowTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(48);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(4);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 48, expectedBlockIndex++);

   When FreezerFilteredTemperatureIs(350);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(20);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(5);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 20, expectedBlockIndex++);

   When FreshFoodFilteredTemperatureIs(0);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(14);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(5);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 14, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(4500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(0);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(5);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);
}

TEST(GridBlockCalculator, ShouldRecalculateBlocksAndAddToPreviousBlocksWhenCalculatedGridLinesChange_TwoDimensionalGrid)
{
   GridBlockNumber_t expectedPreviousBlocks[GridBlockCalculator_NumberOfPreviousGridBlocksToStore] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
   uint8_t expectedBlockIndex = 0;

   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(275);
   And FreezerFilteredTemperatureIs(0);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(38);
   And PreviousGridBlockCountForTwoDimensionalGridShouldBe(0);
   And PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 38, expectedBlockIndex++);

   When CalculatedGridLinesAre(differentCalculatedGridLines);
   CurrentGridBlockForTwoDimensionalGridShouldBe(44);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(1);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);
}

TEST(GridBlockCalculator, ShouldNotUpdatePreviousGridBlocksIfGridBlockDoesntChange_TwoDimensionalGrid)
{
   uint8_t expectedBlockIndex = 0;
   GridBlockNumber_t expectedPreviousBlocks[GridBlockCalculator_NumberOfPreviousGridBlocksToStore] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(275);
   And FreezerFilteredTemperatureIs(0);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(38);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(0);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   When FreshFoodFilteredTemperatureIs(276);
   CurrentGridBlockForTwoDimensionalGridShouldBe(38);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(0);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   When FreshFoodFilteredTemperatureIs(277);
   CurrentGridBlockForTwoDimensionalGridShouldBe(38);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(0);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 38, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(1);
   CurrentGridBlockForTwoDimensionalGridShouldBe(31);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(1);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(2);
   CurrentGridBlockForTwoDimensionalGridShouldBe(31);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(1);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(3);
   CurrentGridBlockForTwoDimensionalGridShouldBe(31);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(1);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   AddBlockToExpectedArray(expectedPreviousBlocks, 31, expectedBlockIndex++);

   When The FreezerFilteredTemperatureIs(5900);
   CurrentGridBlockForTwoDimensionalGridShouldBe(3);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(2);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(5901);
   CurrentGridBlockForTwoDimensionalGridShouldBe(3);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(2);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);

   When The FreezerFilteredTemperatureIs(5902);
   CurrentGridBlockForTwoDimensionalGridShouldBe(3);
   The PreviousGridBlockCountForTwoDimensionalGridShouldBe(2);
   And The PreviousGridBlocksForTwoDimensionalGridShouldBe(expectedPreviousBlocks);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperaturesWithAnInvalidFreshFoodThermistor_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And FreezerThermistorValidityIs(Valid);
   And FreshFoodThermistorValidityIs(Invalid);
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(43);

   When FreshFoodFilteredTemperatureIs(349);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(29);

   When FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(15);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(1);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockAtVariousTemperaturesWithAnInvalidFreezerThermistor_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And FreezerThermistorValidityIs(Invalid);
   And FreshFoodThermistorValidityIs(Valid);
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(27);

   When FreshFoodFilteredTemperatureIs(349);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(24);

   When FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(22);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(21);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreezerThermistorChangesFromValidToInvalidAndBackOnTemperatureChange_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(22);

   When FreezerThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(15);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(0);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreezerThermistorChangesFromValidToInvalidAndBackWithoutTemperatureChange_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(22);

   When FreezerThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(1);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(0);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreshFoodThermistorChangesFromValidToInvalidAndBackOnTemperatureChange_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(FreezerFallbackTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(1);

   When FreshFoodThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(149);
   And FreezerFilteredTemperatureIs(349);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(15);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(0);
}

TEST(GridBlockCalculator, ShouldCalculateCorrectGridBlockWhenFreshFoodThermistorChangesFromValidToInvalidAndBackWithoutTemperatureChange_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(29);

   When FreshFoodThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(34);

   When FreshFoodFilteredTemperatureIs(-100);
   And FreezerFilteredTemperatureIs(2500);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(0);
}

TEST(GridBlockCalculator, ShouldNotCalculateNewGridBlockWhenBothThermistorsChangesFromValidToInvalid_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(29);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(29);
}

TEST(GridBlockCalculator, ShouldRecalculateNewGridBlockWhenOneThermistorChangesBackToValidFromInvalid_TwoDimensionalGrid)
{
   Given CalculatedGridLinesAre(calculatedGridLines);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   And The ModuleIsInitializedWithTwoDimensionalGrid();

   CurrentGridBlockForTwoDimensionalGridShouldBe(48);

   When FreshFoodThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(29);

   When FreezerThermistorValidityIs(Invalid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(149);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(29);

   When FreshFoodThermistorValidityIs(Valid);
   And FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   The CurrentGridBlockForTwoDimensionalGridShouldBe(27);

   When FreshFoodFilteredTemperatureIs(AVeryHighTemp);
   And FreezerFilteredTemperatureIs(AVeryLowTemp);
   And BothThermistorsAreValid();
   The CurrentGridBlockForTwoDimensionalGridShouldBe(48);
}

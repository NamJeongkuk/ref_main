/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CalcGridBlockAndGridLines.h"
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

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
   FivePreviousBlocks = 5,
   Zero = 0
};

static const GridLineErds_t freshFoodErds = {
   .rawSetpointErd = Erd_FreshFoodSetpoint_ResolvedVote,
   .offsetErd = Erd_FreshFood_Offset,
   .shiftErd = Erd_FreshFood_Shift,
   .adjustedSetpointErd = Erd_FreshFood_AdjustedSetpoint
};

static const GridLineErds_t freezerErds = {
   .rawSetpointErd = Erd_FreezerSetpoint_ResolvedVote,
   .offsetErd = Erd_Freezer_Offset,
   .shiftErd = Erd_Freezer_Shift,
   .adjustedSetpointErd = Erd_Freezer_AdjustedSetpoint
};

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b0010,
   },
   {
      .gridLinesDegFx100 = -450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 150,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 950,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 1150,
      .bitMapping = 0b1000,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 600,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 750,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 5500,
      .bitMapping = 0b0010,
   },
};

static const DeltaAxisGridLines_t freshFoodAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freshFoodGridLineData
};

static const DeltaAxisGridLines_t freezerAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freezerGridLineData
};

static DeltaAxisGridLines_t parametricGrid[] = { freshFoodAxis, freezerAxis };
static DeltaGridLines_t deltaGrid = {
   .dimensions = TwoDimensional,
   .gridLines = parametricGrid
};

static const GridData_t gridData = {
   .gridId = 0,
   .deltaGridLines = &deltaGrid,
   .gridPeriodicRunRateInMSec = MSEC_PER_SEC
};

static TemperatureDegFx100_t freshFoodCalcAxisGridLines[SixGridLines] = { 0 };
static TemperatureDegFx100_t freezerCalcAxisGridLines[SixGridLines] = { 0 };

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freshFoodCalcAxisGridLines
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freezerCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .dimensions = TwoDimensional,
   .gridLines = calcGrid
};

static const GridBlockAndLinesConfig_t config = {
   .calculatedGridBlockErd = Erd_Grid_BlockNumber,
   .previousGridBlocksErd = Erd_Grid_PreviousBlocks,
   .calculatedGridLinesErd = Erd_Grid_CalculatedGridLines,
   .freshFoodFilteredTempErd = Erd_FreshFood_FilteredTemperature,
   .freezerFilteredTempErd = Erd_Freezer_FilteredTemperature,
   .timerModuleErd = Erd_TimerModule,
   .freezerErds = freezerErds,
   .freshFoodErds = freshFoodErds
};

static GridBlockNumber_t ringBufferNumbers[FivePreviousBlocks] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static PreviousGridBlockNumbers_t prevGridBlocks = {
   .maxSize = FivePreviousBlocks,
   .currentNumberOfBlocksStored = Zero
};

TEST_GROUP(CalcGridBlockAndGridLines)
{
   CalcGridBlockAndLines_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   RingBuffer_t bufferInstance;
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

   void teardown()
   {
      for(uint8_t i = 0; i < calcGridLines.dimensions; i++)
      {
         for(uint8_t j = 0; j < calcGridLines.gridLines[i].numberOfLines; j++)
         {
            calcGridLines.gridLines[i].gridLinesDegFx100[j] = 0;
         }
      }
   }

   void WhenTheModuleIsInitialized()
   {
      RingBufferIsInitialized();
      CalcGridBlockAndGridLines_Init(
         &instance,
         &config,
         dataModel,
         &calcGridLines,
         &bufferInstance,
         &prevGridBlocks);
   }

   void RingBufferIsInitialized()
   {
      RingBuffer_Init(&bufferInstance, ringBufferNumbers, prevGridBlocks.maxSize, ELEMENT_SIZE(ringBufferNumbers));
   }

   void CalculatedGridLinesShouldBeZero()
   {
      CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_Nfl);
      And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodLowHyst);
      And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodLowHystDelta);
      And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodHighHyst);
      And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodExtraHigh);
      And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodSuperHigh);

      And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerLowHyst);
      And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerDelta);
      And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerHighHyst);
      And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerExtraHigh);
      And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerSuperHigh);
      And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerExtremeHigh);
   }

   void NothingShouldHappen()
   {
   }

   void GivenTheFreshFoodAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_FreshFood_AdjustedSetpoint, &setpoint);
   }

   void GivenTheFreezerAdjustedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_Freezer_AdjustedSetpoint, &setpoint);
   }

   void GivenAdjustedSetpointsAreInitialized()
   {
      GivenTheFreshFoodAdjustedSetpointIs(3890);
      GivenTheFreezerAdjustedSetpointIs(440);
   }

   void GivenTheFreshFoodOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel, Erd_FreshFood_Offset, &offset);
   }

   void GivenTheFreezerOffsetIs(TemperatureDegFx100_t offset)
   {
      DataModel_Write(dataModel, Erd_Freezer_Offset, &offset);
   }

   void GivenOffsetsAreInitialized()
   {
      GivenTheFreshFoodOffsetIs(190);
      GivenTheFreezerOffsetIs(450);
   }

   void GivenGridErdsAreInitialized()
   {
      GivenAdjustedSetpointsAreInitialized();
      GivenOffsetsAreInitialized();
   }

   void CalculatedGridLineTempShouldBe(TemperatureDegFx100_t temp, int index, int index2)
   {
      CalculatedGridLines_t calcLines;
      DataModel_Read(dataModel, Erd_Grid_CalculatedGridLines, &calcLines);
      CHECK_EQUAL(temp, calcLines.gridLines[index].gridLinesDegFx100[index2]);
   }

   void CalculatedGridBlockShouldBe(GridBlockNumber_t blockNumber)
   {
      GridBlockNumber_t actualBlockNumber;
      DataModel_Read(dataModel, Erd_Grid_BlockNumber, &actualBlockNumber);
      CHECK_EQUAL(blockNumber, actualBlockNumber);
   }

   void FreshFoodFilteredTempIs(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperature, &temp);
   }

   void FreezerFilteredTempIs(TemperatureDegFx100_t temp)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperature, &temp);
   }

   TemperatureDegFx100_t GridLineTemp(int compartmentType, int gridLineType)
   {
      CalculatedGridLines_t calcLines;
      DataModel_Read(dataModel, Erd_Grid_CalculatedGridLines, &calcLines);

      return calcLines.gridLines[compartmentType].gridLinesDegFx100[gridLineType];
   }

   void GivenTheGridBlockIsNow(GridBlockNumber_t blockNumber)
   {
      DataModel_Write(dataModel, Erd_Grid_BlockNumber, &blockNumber);
   }

   void PreviousGridBlocksArrayShouldBe(GridBlockNumber_t firstBlock, GridBlockNumber_t secondBlock, GridBlockNumber_t thirdBlock, GridBlockNumber_t fourthBlock, GridBlockNumber_t fifthBlock)
   {
      PreviousGridBlockNumbers_t prevBlocks;
      DataModel_Read(dataModel, Erd_Grid_PreviousBlocks, &prevBlocks);

      GridBlockNumber_t blocks[FivePreviousBlocks] = { firstBlock, secondBlock, thirdBlock, fourthBlock, fifthBlock };

      for(unsigned i = 0; i < prevBlocks.currentNumberOfBlocksStored; i++)
      {
         CHECK_EQUAL(blocks[i], prevBlocks.blockNumbers[i]);
      }
   }

   void PreviousGridBlocksSizeShouldBe(unsigned size)
   {
      PreviousGridBlockNumbers_t prevBlocks;

      DataModel_Read(dataModel, Erd_Grid_PreviousBlocks, &prevBlocks);
      CHECK_EQUAL(size, prevBlocks.currentNumberOfBlocksStored);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }
};

TEST(CalcGridBlockAndGridLines, ShouldInitCalculatedGridLinesToZero)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();

   CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_Nfl);
   And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodLowHyst);
   And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodLowHystDelta);
   And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodHighHyst);
   And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodExtraHigh);
   And CalculatedGridLineTempShouldBe(0, GridDelta_FreshFood, GridLine_FreshFoodSuperHigh);

   And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerLowHyst);
   And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerDelta);
   And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerHighHyst);
   And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerExtraHigh);
   And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerSuperHigh);
   And CalculatedGridLineTempShouldBe(0, GridDelta_Freezer, GridLine_FreezerExtremeHigh);
}

TEST(CalcGridBlockAndGridLines, ShouldCalculateGridLinesAfterOneSecond)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();

   After(MSEC_PER_SEC - 1);
   CalculatedGridLinesShouldBeZero();

   After(1);
   CalculatedGridLineTempShouldBe(190, GridDelta_FreshFood, GridLine_Nfl);
   And CalculatedGridLineTempShouldBe(3440, GridDelta_FreshFood, GridLine_FreshFoodLowHyst);
   And CalculatedGridLineTempShouldBe(4040, GridDelta_FreshFood, GridLine_FreshFoodLowHystDelta);
   And CalculatedGridLineTempShouldBe(4340, GridDelta_FreshFood, GridLine_FreshFoodHighHyst);
   And CalculatedGridLineTempShouldBe(4840, GridDelta_FreshFood, GridLine_FreshFoodExtraHigh);
   And CalculatedGridLineTempShouldBe(5040, GridDelta_FreshFood, GridLine_FreshFoodSuperHigh);

   And CalculatedGridLineTempShouldBe(190, GridDelta_Freezer, GridLine_FreezerLowHyst);
   And CalculatedGridLineTempShouldBe(440, GridDelta_Freezer, GridLine_FreezerDelta);
   And CalculatedGridLineTempShouldBe(690, GridDelta_Freezer, GridLine_FreezerHighHyst);
   And CalculatedGridLineTempShouldBe(1040, GridDelta_Freezer, GridLine_FreezerExtraHigh);
   And CalculatedGridLineTempShouldBe(1190, GridDelta_Freezer, GridLine_FreezerSuperHigh);
   And CalculatedGridLineTempShouldBe(5950, GridDelta_Freezer, GridLine_FreezerExtremeHigh);
}

TEST(CalcGridBlockAndGridLines, ShouldReCalculateGridLinesAfterOneSecondWhenAdjSetpointsChange)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();

   After(MSEC_PER_SEC - 1);
   CalculatedGridLinesShouldBeZero();

   After(1);
   CalculatedGridLineTempShouldBe(190, GridDelta_FreshFood, GridLine_Nfl);
   And CalculatedGridLineTempShouldBe(3440, GridDelta_FreshFood, GridLine_FreshFoodLowHyst);
   And CalculatedGridLineTempShouldBe(4040, GridDelta_FreshFood, GridLine_FreshFoodLowHystDelta);
   And CalculatedGridLineTempShouldBe(4340, GridDelta_FreshFood, GridLine_FreshFoodHighHyst);
   And CalculatedGridLineTempShouldBe(4840, GridDelta_FreshFood, GridLine_FreshFoodExtraHigh);
   And CalculatedGridLineTempShouldBe(5040, GridDelta_FreshFood, GridLine_FreshFoodSuperHigh);

   And CalculatedGridLineTempShouldBe(190, GridDelta_Freezer, GridLine_FreezerLowHyst);
   And CalculatedGridLineTempShouldBe(440, GridDelta_Freezer, GridLine_FreezerDelta);
   And CalculatedGridLineTempShouldBe(690, GridDelta_Freezer, GridLine_FreezerHighHyst);
   And CalculatedGridLineTempShouldBe(1040, GridDelta_Freezer, GridLine_FreezerExtraHigh);
   And CalculatedGridLineTempShouldBe(1190, GridDelta_Freezer, GridLine_FreezerSuperHigh);
   And CalculatedGridLineTempShouldBe(5950, GridDelta_Freezer, GridLine_FreezerExtremeHigh);

   GivenTheFreshFoodAdjustedSetpointIs(4690);
   And GivenTheFreezerAdjustedSetpointIs(-60);

   After(MSEC_PER_SEC);
   CalculatedGridLineTempShouldBe(190, GridDelta_FreshFood, GridLine_Nfl);
   And CalculatedGridLineTempShouldBe(4240, GridDelta_FreshFood, GridLine_FreshFoodLowHyst);
   And CalculatedGridLineTempShouldBe(4840, GridDelta_FreshFood, GridLine_FreshFoodLowHystDelta);
   And CalculatedGridLineTempShouldBe(5140, GridDelta_FreshFood, GridLine_FreshFoodHighHyst);
   And CalculatedGridLineTempShouldBe(5640, GridDelta_FreshFood, GridLine_FreshFoodExtraHigh);
   And CalculatedGridLineTempShouldBe(5840, GridDelta_FreshFood, GridLine_FreshFoodSuperHigh);

   And CalculatedGridLineTempShouldBe(-310, GridDelta_Freezer, GridLine_FreezerLowHyst);
   And CalculatedGridLineTempShouldBe(-60, GridDelta_Freezer, GridLine_FreezerDelta);
   And CalculatedGridLineTempShouldBe(190, GridDelta_Freezer, GridLine_FreezerHighHyst);
   And CalculatedGridLineTempShouldBe(540, GridDelta_Freezer, GridLine_FreezerExtraHigh);
   And CalculatedGridLineTempShouldBe(690, GridDelta_Freezer, GridLine_FreezerSuperHigh);
   And CalculatedGridLineTempShouldBe(5950, GridDelta_Freezer, GridLine_FreezerExtremeHigh);
}

TEST(CalcGridBlockAndGridLines, PreviousGridBlocksShouldBeEmptyOnInit)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4040);
   And FreezerFilteredTempIs(690);

   PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, PreviousGridBlocksShouldBeEmptyAfterGridBlockErdChangesOnce)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4040);
   And FreezerFilteredTempIs(690);

   PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);

   GivenTheGridBlockIsNow(0);
   PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, PreviousGridBlocksShouldHaveOneElementAfterGridBlockErdChangesTwice)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4040);
   And FreezerFilteredTempIs(690);

   PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);

   GivenTheGridBlockIsNow(0);
   PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);

   GivenTheGridBlockIsNow(1);
   PreviousGridBlocksArrayShouldBe(0, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(1);
}

TEST(CalcGridBlockAndGridLines, PreviousGridBlocksShouldStorePreviousFiveBlockNumbersAndOnSixthNewBlockNumberReplaceOldestBlockNumber)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4040);
   And FreezerFilteredTempIs(690);

   GivenTheGridBlockIsNow(0);
   GivenTheGridBlockIsNow(1);
   GivenTheGridBlockIsNow(2);
   GivenTheGridBlockIsNow(3);
   GivenTheGridBlockIsNow(4);
   GivenTheGridBlockIsNow(5);
   PreviousGridBlocksArrayShouldBe(0, 1, 2, 3, 4);
   And PreviousGridBlocksSizeShouldBe(5);

   GivenTheGridBlockIsNow(6);
   PreviousGridBlocksArrayShouldBe(1, 2, 3, 4, 5);
   And PreviousGridBlocksSizeShouldBe(5);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock42WhenFreshFoodAndFreezerTempAreZero)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(0);
   And FreezerFilteredTempIs(0);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(42);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock0WhenBelowFreshFoodNflAndAboveFreezerExtremeHigh)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(189);
   And FreezerFilteredTempIs(5951);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(0);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock48WhenAboveFreshFoodSuperHighAndBelowFreezerLowHyst)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(5041);
   And FreezerFilteredTempIs(189);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(48);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock6WhenAboveFreshFoodSuperHighAndAboveFreezerExtremeHigh)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(5041);
   And FreezerFilteredTempIs(5951);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(6);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock25WhenAboveFreshFoodHighHystAndAboveFreezerHighHyst)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4341);
   And FreezerFilteredTempIs(691);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(25);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock30WhenEqualToFreshFoodLowHystDeltaAndEqualToFreezerHighHyst)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4040);
   And FreezerFilteredTempIs(690);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(30);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock46WhenAboveFreshFoodHighHystAndBelowFreezerLowHyst)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(4341);
   And FreezerFilteredTempIs(189);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(46);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);
}

TEST(CalcGridBlockAndGridLines, ShouldBeInGridBlock14WhenBelowFreshFoodNflAndAboveFreezerExtraHighThenChangeToGridBlock15WhenTempsChange)
{
   GivenGridErdsAreInitialized();
   WhenTheModuleIsInitialized();
   And FreshFoodFilteredTempIs(189);
   And FreezerFilteredTempIs(1041);

   After(MSEC_PER_SEC - 1);
   NothingShouldHappen();

   After(1);
   CalculatedGridBlockShouldBe(14);
   And PreviousGridBlocksArrayShouldBe(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(0);

   After(MSEC_PER_SEC - 1);
   FreshFoodFilteredTempIs(191);
   And FreezerFilteredTempIs(1041);

   After(1);
   CalculatedGridBlockShouldBe(15);
   And PreviousGridBlocksArrayShouldBe(14, 0xFF, 0xFF, 0xFF, 0xFF);
   And PreviousGridBlocksSizeShouldBe(1);
}

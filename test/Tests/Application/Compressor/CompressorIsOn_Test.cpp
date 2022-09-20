/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorIsOn.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

TEST_GROUP(CompressorIsOn)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void CompressorIsOnIsInitialized()
   {
      CompressorIsOn_Init(dataModel);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void CompressorIsOnShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_CompressorIsOn, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }
};

TEST(CompressorIsOn, ShouldInitialize)
{
   CompressorIsOnIsInitialized();
}

TEST(CompressorIsOn, ShouldBeFalseWhenStateIsRemainOffAfterValveMove)
{
   Given CompressorStateIs(CompressorState_RemainOffAfterValveMove);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(false);
}

TEST(CompressorIsOn, ShouldBeFalseWhenStateIsOffAndReadyToChange)
{
   Given CompressorStateIs(CompressorState_OffAndReadyToChange);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(false);
}

TEST(CompressorIsOn, ShouldBeTrueWhenStateIsOnAndReadyToChange)
{
   Given CompressorStateIs(CompressorState_OnAndReadyToChange);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(true);
}

TEST(CompressorIsOn, ShouldBeTrueWhenStateIsMinimumOnTime)
{
   Given CompressorStateIs(CompressorState_MinimumOnTime);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(true);
}

TEST(CompressorIsOn, ShouldBeFalseWhenStateIsMinimumOffTime)
{
   Given CompressorStateIs(CompressorState_MinimumOffTime);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(false);
}

TEST(CompressorIsOn, ShouldBeFalseWhenStateIsOutOfBounds)
{
   Given CompressorStateIs(CompressorState_Max);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(false);
}

TEST(CompressorIsOn, ShouldChangeToTrueWhenFalseAndCompressorTurnsOn)
{
   Given CompressorStateIs(CompressorState_OffAndReadyToChange);
   And CompressorIsOnIsInitialized();
   CompressorIsOnShouldBe(false);

   When CompressorStateIs(CompressorState_MinimumOnTime);
   CompressorIsOnShouldBe(true);
}

TEST(CompressorIsOn, ShouldChangeToFalseWhenTrueAndCompressorTurnsOff)
{
   Given CompressorStateIs(CompressorState_OnAndReadyToChange);
   And CompressorIsOnIsInitialized();
   CompressorIsOnShouldBe(true);

   When CompressorStateIs(CompressorState_MinimumOffTime);
   CompressorIsOnShouldBe(false);
}

TEST(CompressorIsOn, ShouldBeTrueWhenStateIsOn)
{
   Given CompressorStateIs(CompressorState_On);
   And CompressorIsOnIsInitialized();

   CompressorIsOnShouldBe(true);
}

TEST(CompressorIsOn, ShouldBeFalseWhenStateIsOff)
{
   Given CompressorStateIs(CompressorState_On);
   And CompressorIsOnIsInitialized();
   CompressorIsOnShouldBe(true);

   When CompressorStateIs(CompressorState_Off);
   CompressorIsOnShouldBe(false);
}

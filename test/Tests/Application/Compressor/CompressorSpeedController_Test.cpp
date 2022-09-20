/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorSpeedController.h"
#include "DataModelErdPointerAccess.h"
#include "CompressorData.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "CompressorSpeedFrequencyInHz.h"

#define Given
#define When
#define And

static const CompressorSpeedControllerConfiguration_t config = {
   .compressorStateErd = Erd_CompressorState,
   .compressorSpeedRequestErd = Erd_CompressorSpeedRequestInHz,
   .compressorSpeedResolvedVoteErd = Erd_CompressorSpeed_ResolvedVote
};

TEST_GROUP(CompressorSpeedController)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t *personalityParametricData;
   const CompressorData_t *compressorData;
   CompressorSpeedController_t instance;

   TimerModule_t *timerModule;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Read(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
      compressorData = personalityParametricData->compressorData;
   }

   void TheCompressorSpeedControllerIsInitialized()
   {
      CompressorSpeedController_Init(&instance, dataModel, &config);
   }

   void CompressorSpeedRequestShouldBe(CompressorSpeedFrequencyInHz_t expected)
   {
      CompressorSpeedFrequencyInHz_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeedRequestInHz, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheCompressorStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(dataModel, Erd_CompressorState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ResolvedCompressorSpeedVoteIs(CompressorSpeed_t speed)
   {
      CompressorVotedSpeed_t vote = {
         .speed = speed,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_CompressorSpeed_ResolvedVote, &vote);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }
};

TEST(CompressorSpeedController, ShouldSetCompressorStateAsStartupOnInitialization)
{
   Given TheCompressorSpeedControllerIsInitialized();

   TheCompressorStateShouldBe(CompressorState_Startup);
}

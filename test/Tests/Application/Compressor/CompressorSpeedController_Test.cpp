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
#include "TimerModule_TestDouble.h"
#include "CompressorSpeedFrequencyInHz.h"

#define Given
#define When
#define The
#define And

static const CompressorSpeedControllerConfiguration_t config = {
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

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Read(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
      compressorData = personalityParametricData->compressorData;
   }

   void CompressorSpeedControllerIsInitialized()
   {
      CompressorSpeedController_Init(
         &instance,
         dataModel,
         DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
         &config);
   }

   void CompressorSpeedRequestShouldBe(CompressorSpeedFrequencyInHz_t expected)
   {
      CompressorSpeedFrequencyInHz_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeedRequestInHz, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(CompressorSpeedController, ShouldInitialize)
{
   Given The CompressorSpeedControllerIsInitialized();
}

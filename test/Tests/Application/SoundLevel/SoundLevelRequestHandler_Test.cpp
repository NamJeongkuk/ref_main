/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SoundLevelRequestHandler.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

static const SoundLevelRequestHandlerConfig_t config = {
   .soundLevelRequestErd = Erd_UiSoundLevelRequest,
   .soundLevelStateErd = Erd_UiSoundLevelState,
   .numberOfSoundLevelSettingsErd = Erd_NumberOfSoundLevelSettings
};

TEST_GROUP(SoundLevelRequestHandler)
{
   SoundLevelRequestHandler_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      SoundLevelRequestHandler_Init(
         &instance,
         dataModel,
         &config);
   }

   void WhenSoundLevelRequestIs(uint8_t request)
   {
      DataModel_Write(dataModel, Erd_UiSoundLevelRequest, &request);
   }

   void SoundLevelStateShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_UiSoundLevelState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void SoundLevelRequestShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_UiSoundLevelRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenNumberOfSoundLevelSettingsIs(uint8_t number)
   {
      DataModel_Write(dataModel, Erd_NumberOfSoundLevelSettings, &number);
   }
};

TEST(SoundLevelRequestHandler, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(SoundLevelRequestHandler, ShouldSoundLevelRequestBe0xFFOnInit)
{
   GivenTheModuleIsInitialized();
   SoundLevelRequestShouldBe(0xFF);
}

TEST(SoundLevelRequestHandler, ShouldCopySoundLevelRequestToTheStatus)
{
   GivenTheModuleIsInitialized();
   GivenNumberOfSoundLevelSettingsIs(3);
   SoundLevelStateShouldBe(1);

   WhenSoundLevelRequestIs(0);
   SoundLevelStateShouldBe(0);

   WhenSoundLevelRequestIs(1);
   SoundLevelStateShouldBe(1);

   WhenSoundLevelRequestIs(2);
   SoundLevelStateShouldBe(2);
}

TEST(SoundLevelRequestHandler, ShouldClearSoundLevelRequestTo0xFFAfterRequested)
{
   GivenTheModuleIsInitialized();

   WhenSoundLevelRequestIs(1);
   SoundLevelRequestShouldBe(0xFF);
}

TEST(SoundLevelRequestHandler, ShouldNotCopySoundLevelRequestToTheStateWhenTheRequestIsGreaterThanOrEqualToNumberOfSoundLevelSettings)
{
   GivenTheModuleIsInitialized();
   GivenNumberOfSoundLevelSettingsIs(3);
   SoundLevelStateShouldBe(1);

   WhenSoundLevelRequestIs(3);
   SoundLevelStateShouldBe(1);

   WhenSoundLevelRequestIs(4);
   SoundLevelStateShouldBe(1);
}

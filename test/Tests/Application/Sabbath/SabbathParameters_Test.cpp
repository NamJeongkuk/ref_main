/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SabbathParameters.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 16 * 60
};

static const SabbathParametersConfig_t config = {
   .sabbathModeErd = Erd_SabbathMode,
   .maxTimeBetweenDefrostsInMinutesErd = Erd_MaxTimeBetweenDefrostsInMinutes
};

TEST_GROUP(SabbathParameters)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;
   SabbathParameters_t instance;
   const DefrostData_t *defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void SabbathParametersAreInitialized()
   {
      SabbathParameters_Init(&instance, dataModel, &config);
   }

   void SabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathMode, &state);
   }

   void MaxTimeBetweenDefrostsInMinutesIs(uint16_t minutes)
   {
      DataModel_Write(dataModel, Erd_MaxTimeBetweenDefrostsInMinutes, &minutes);
   }

   void MaxTimeBetweenDefrostsInMinutesShouldBe(uint16_t expectedMinutes)
   {
      uint16_t actualMinutes;
      DataModel_Read(dataModel, Erd_MaxTimeBetweenDefrostsInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void SabbathAndSabbathParametersAreEnabled()
   {
      Given MaxTimeBetweenDefrostsInMinutesIs(0);
      And SabbathModeIs(ENABLED);

      When SabbathParametersAreInitialized();
      MaxTimeBetweenDefrostsInMinutesShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes);
   }

   void SabbathAndSabbathParametersAreDisabled()
   {
      Given MaxTimeBetweenDefrostsInMinutesIs(0);
      And SabbathModeIs(DISABLED);

      When SabbathParametersAreInitialized();
      MaxTimeBetweenDefrostsInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes);
   }
};

TEST(SabbathParameters, ShouldInitialize)
{
   Given SabbathParametersAreInitialized();
}

TEST(SabbathParameters, ShouldSetMaxTimeBetweenDefrostsToNormalTimeWhenSabbathIsDisabledOnInit)
{
   Given MaxTimeBetweenDefrostsInMinutesIs(0);
   And SabbathModeIs(DISABLED);

   When SabbathParametersAreInitialized();
   MaxTimeBetweenDefrostsInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes);
}

TEST(SabbathParameters, ShouldSetMaxTimeBetweenDefrostsToSabbathTimeWhenSabbathIsEnabledOnInit)
{
   Given MaxTimeBetweenDefrostsInMinutesIs(0);
   And SabbathModeIs(ENABLED);

   When SabbathParametersAreInitialized();
   MaxTimeBetweenDefrostsInMinutesShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes);
}

TEST(SabbathParameters, ShouldUpdateMaxTimeBetweenDefrostsToNormalTimeWhenSabbathIsDisabled)
{
   Given SabbathAndSabbathParametersAreEnabled();

   When SabbathModeIs(DISABLED);
   MaxTimeBetweenDefrostsInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes);
}

TEST(SabbathParameters, ShouldSetMaxTimeBetweenDefrostsToSabbathTimeWhenSabbathIsEnabled)
{
   Given SabbathAndSabbathParametersAreDisabled();

   When SabbathModeIs(ENABLED);
   MaxTimeBetweenDefrostsInMinutesShouldBe(sabbathData.maxTimeBetweenDefrostsInMinutes);
}

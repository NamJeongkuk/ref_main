/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanModeSetpointResolver.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   ResetValue = INT16_MAX,

   SomeMinimumTemperature = 1,
   SomeDefaultTemperature = 5,
   SomeMaximumTemperature = 10,

   SomeOtherMinimumTemperature = 20,
   SomeOtherDefaultTemperature = 30,
   SomeOtherMaximumTemperature = 40,
};

static const FeaturePanModeSetpointResolverConfigurationEntry_t entries[] = {
   { .setpointRequestErd = Erd_FeaturePanMode1_Request,
      .setpointStatusErd = Erd_FeaturePanMode1_Status,
      .setpointRangeErd = Erd_FeaturePanMode1_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode2_Request,
      .setpointStatusErd = Erd_FeaturePanMode2_Status,
      .setpointRangeErd = Erd_FeaturePanMode2_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode3_Request,
      .setpointStatusErd = Erd_FeaturePanMode3_Status,
      .setpointRangeErd = Erd_FeaturePanMode3_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode4_Request,
      .setpointStatusErd = Erd_FeaturePanMode4_Status,
      .setpointRangeErd = Erd_FeaturePanMode4_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode5_Request,
      .setpointStatusErd = Erd_FeaturePanMode5_Status,
      .setpointRangeErd = Erd_FeaturePanMode5_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode6_Request,
      .setpointStatusErd = Erd_FeaturePanMode6_Status,
      .setpointRangeErd = Erd_FeaturePanMode6_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode7_Request,
      .setpointStatusErd = Erd_FeaturePanMode7_Status,
      .setpointRangeErd = Erd_FeaturePanMode7_SetpointRangeData },
};

static const FeaturePanModeSetpointResolverConfiguration_t config = {
   .entries = entries,
   .numberOfEntries = NUM_ELEMENTS(entries)
};

static const FeaturePanModeRange_t someSetpointRangeData = {
   .minTemperature = SomeMinimumTemperature,
   .maxTemperature = SomeMaximumTemperature,
   .defaultTemperature = SomeDefaultTemperature,
   .mode = FeaturePanModeLabel_Freezer
};

TEST_GROUP(FeaturePanModeSetpointResolver)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   FeaturePanModeSetpointResolver_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      FeaturePanModeSetpointResolver_Init(&instance, dataModel, &config);
   }

   void WhenTheErdIsWrittenWithTemperature(Erd_t erd, TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, erd, &temperature);
   }

   void WhenTheSetpointRangeErdIsWrittenWith(Erd_t erd, const FeaturePanModeRange_t *rangeData)
   {
      DataModel_Write(dataModel, erd, rangeData);
   }

   void GivenTheSetpointStatusErdIsWrittenWith(Erd_t erd, TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, erd, &temperature);
   }

   void GivenAllSetpointStatusErdsAreWrittenWith(TemperatureDegFx100_t temperature)
   {
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode1_Status, temperature);
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode2_Status, temperature);
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode3_Status, temperature);
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode4_Status, temperature);
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode5_Status, temperature);
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode6_Status, temperature);
      GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode7_Status, temperature);
   }

   void GivenAllSetpointRangeErdsAreWrittenWith(const FeaturePanModeRange_t *rangeData)
   {
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode1_SetpointRangeData, rangeData);
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode2_SetpointRangeData, rangeData);
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode3_SetpointRangeData, rangeData);
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode4_SetpointRangeData, rangeData);
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode5_SetpointRangeData, rangeData);
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode6_SetpointRangeData, rangeData);
      WhenTheSetpointRangeErdIsWrittenWith(Erd_FeaturePanMode7_SetpointRangeData, rangeData);
   }

   void TheSetpointStatusErdShouldBe(Erd_t erd, TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, erd, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void AllSetpointStatusErdsShouldBe(TemperatureDegFx100_t expected)
   {
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode1_Status, expected);
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode2_Status, expected);
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode3_Status, expected);
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode4_Status, expected);
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode5_Status, expected);
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode6_Status, expected);
      TheSetpointStatusErdShouldBe(Erd_FeaturePanMode7_Status, expected);
   }
};

TEST(FeaturePanModeSetpointResolver, ShouldResolveAllSetpointsToDefaultTemperatureWhenInitializedWithoutDataSavedInEeprom)
{
   GivenAllSetpointStatusErdsAreWrittenWith(ResetValue);
   GivenAllSetpointRangeErdsAreWrittenWith(&someSetpointRangeData);
   GivenTheModuleIsInitialized();

   AllSetpointStatusErdsShouldBe(SomeDefaultTemperature);
}

TEST(FeaturePanModeSetpointResolver, ShouldNotResolveAllSetpointsToDefaultTemperatureWhenInitializedWithDataSavedInEeprom)
{
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode1_Status, 0x1111);
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode2_Status, ResetValue);
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode3_Status, 0x3333);
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode4_Status, ResetValue);
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode5_Status, 0x5555);
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode6_Status, ResetValue);
   GivenTheSetpointStatusErdIsWrittenWith(Erd_FeaturePanMode7_Status, 0x7777);

   GivenAllSetpointRangeErdsAreWrittenWith(&someSetpointRangeData);
   GivenTheModuleIsInitialized();

   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode1_Status, 0x1111);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode2_Status, SomeDefaultTemperature);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode3_Status, 0x3333);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode4_Status, SomeDefaultTemperature);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode5_Status, 0x5555);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode6_Status, SomeDefaultTemperature);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode7_Status, 0x7777);
}

TEST(FeaturePanModeSetpointResolver, ShouldResolveSetpointWhenRequestChanges)
{
   GivenAllSetpointStatusErdsAreWrittenWith(ResetValue);
   GivenAllSetpointRangeErdsAreWrittenWith(&someSetpointRangeData);
   GivenTheModuleIsInitialized();

   AllSetpointStatusErdsShouldBe(SomeDefaultTemperature);

   WhenTheErdIsWrittenWithTemperature(Erd_FeaturePanMode1_Request, SomeDefaultTemperature - 1);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode1_Status, SomeDefaultTemperature - 1);
}

TEST(FeaturePanModeSetpointResolver, ShouldClampSetpointToMinOrMaxWhenRequestChangesOutsideOfRange)
{
   GivenAllSetpointStatusErdsAreWrittenWith(ResetValue);
   GivenAllSetpointRangeErdsAreWrittenWith(&someSetpointRangeData);
   GivenTheModuleIsInitialized();

   AllSetpointStatusErdsShouldBe(SomeDefaultTemperature);

   WhenTheErdIsWrittenWithTemperature(Erd_FeaturePanMode1_Request, SomeMaximumTemperature + 1);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode1_Status, SomeMaximumTemperature);

   WhenTheErdIsWrittenWithTemperature(Erd_FeaturePanMode1_Request, SomeMinimumTemperature - 1);
   TheSetpointStatusErdShouldBe(Erd_FeaturePanMode1_Status, SomeMinimumTemperature);
}

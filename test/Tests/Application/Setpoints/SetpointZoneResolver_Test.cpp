/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SetpointZoneResolver.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define Then

enum
{
   TemperatureBelowColdSetpointBoundInDegFx100 = 0,
   TemperatureAboveWarmSetpointBoundInDegFx100 = 500,
   TemperatureBetweenMiddleSetpointBoundsInDegFx100 = 250,
   SetpointTemperatureColdBoundInDegFx100 = 200,
   SetpointTemperatureWarmBoundInDegFx100 = 300,
};

static const SetpointZoneResolverConfig_t setpointZoneResolverConfig = {
   .setpointVotedTemperatureErd = Erd_FreezerSetpoint_ResolvedVote,
   .setpointZoneErd = Erd_FreezerSetpointZone,
   .setpointZoneTemperatureBoundsErd = Erd_FreezerSetpoint_TemperatureBounds,
};

TEST_GROUP(SetpointZoneResolver)
{
   I_DataModel_t *dataModel;
   SetpointZoneResolver_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      SetpointZoneResolver_Init(&instance, &setpointZoneResolverConfig, dataModel);
   }

   void TheSetpointTemperatureIs(TemperatureDegFx100_t setpointTemperature)
   {
      SetpointVotedTemperature_t votedSetpointTemperature = {
         .temperatureInDegFx100 = setpointTemperature,
         .care = true
      };

      DataModel_Write(dataModel, Erd_FreezerSetpoint_ResolvedVote, &votedSetpointTemperature);
   }

   void TheSetpointZoneShouldBe(SetpointZone_t expected)
   {
      SetpointZone_t actual;
      DataModel_Read(dataModel, Erd_FreezerSetpointZone, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheSetpointTemperatureBoundsAre(TemperatureDegFx100_t coldSetpointBoundInDegFx100, TemperatureDegFx100_t warmSetpointBoundInDegFx100)
   {
      SetpointZoneTemperatureBounds_t setpointTemperattureBounds = {
         .coldSetpointBoundInDegFx100 = coldSetpointBoundInDegFx100,
         .warmSetpointBoundInDegFx100 = warmSetpointBoundInDegFx100
      };

      DataModel_Write(
         dataModel,
         Erd_FreezerSetpoint_TemperatureBounds,
         &setpointTemperattureBounds);
   }
};

TEST(SetpointZoneResolver, ShouldSetSetpointZoneAccordingToSetpointTemperatureResolvedVoteErd)
{
   Given TheModuleIsInitialized();
   And TheSetpointTemperatureBoundsAre(SetpointTemperatureColdBoundInDegFx100, SetpointTemperatureWarmBoundInDegFx100);

   When TheSetpointTemperatureIs(TemperatureBelowColdSetpointBoundInDegFx100);
   Then TheSetpointZoneShouldBe(SetpointZone_Cold);

   When TheSetpointTemperatureIs(TemperatureBetweenMiddleSetpointBoundsInDegFx100);
   Then TheSetpointZoneShouldBe(SetpointZone_Middle);

   When TheSetpointTemperatureIs(TemperatureAboveWarmSetpointBoundInDegFx100);
   Then TheSetpointZoneShouldBe(SetpointZone_Warm);
}

TEST(SetpointZoneResolver, ShouldSetSetpointZoneWatchErdWhenSetBeforeInitForWarm)
{
   Given TheSetpointTemperatureIs(TemperatureAboveWarmSetpointBoundInDegFx100);
   And TheModuleIsInitialized();
   And TheSetpointTemperatureBoundsAre(SetpointTemperatureColdBoundInDegFx100, SetpointTemperatureWarmBoundInDegFx100);

   Then TheSetpointZoneShouldBe(SetpointZone_Warm);
}

TEST(SetpointZoneResolver, ShouldSetSetpointZoneWatchErdWhenSetBeforeInitForCold)
{
   Given TheSetpointTemperatureIs(TemperatureBelowColdSetpointBoundInDegFx100);
   And TheSetpointTemperatureBoundsAre(SetpointTemperatureColdBoundInDegFx100, SetpointTemperatureWarmBoundInDegFx100);
   And TheModuleIsInitialized();

   Then TheSetpointZoneShouldBe(SetpointZone_Cold);
}

TEST(SetpointZoneResolver, ShouldSetSetpointZoneWatchErdWhenSetBeforeInitForMiddle)
{
   Given TheSetpointTemperatureIs(TemperatureBetweenMiddleSetpointBoundsInDegFx100);
   And TheSetpointTemperatureBoundsAre(SetpointTemperatureColdBoundInDegFx100, SetpointTemperatureWarmBoundInDegFx100);
   And TheModuleIsInitialized();

   Then TheSetpointZoneShouldBe(SetpointZone_Middle);
}

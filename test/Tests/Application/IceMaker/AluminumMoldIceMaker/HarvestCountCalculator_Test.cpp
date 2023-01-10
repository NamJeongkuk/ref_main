/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "HarvestCountCalculator.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"

#define TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes \
   (aluminumMoldIceMakerData->freezeData.startIntegrationTemperatureInDegFx100 -           \
      aluminumMoldIceMakerData->freezeData.freezeIntegrationLimitInDegFx100TimesSeconds / (aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * SECONDS_PER_MINUTE))

static const HarvestCountCalculatorConfiguration_t config = {
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100
};

TEST_GROUP(HarvestCountCalculator)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t *personalityParametricData;
   const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
   HarvestCountCalculator_t instance;
   TimerModule_t *timerModule;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Read(
         dataModel,
         Erd_PersonalityParametricData,
         &personalityParametricData);
      aluminumMoldIceMakerData = personalityParametricData->iceMakerData->aluminumMoldIceMakerData;
   }

   void GivenTheModuleIsInitialized()
   {
      HarvestCountCalculator_Init(&instance, dataModel, &config, aluminumMoldIceMakerData);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void GivenTheHarvestCountCalculationRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_HarvestCountCalculationRequest, &state);
   }

   void GivenTheIceMakerTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenTheIceMakerTemperatureIs(TemperatureDegFx100_t temperature)
   {
      GivenTheIceMakerTemperatureIs(temperature);
   }

   void GivenHarvestCountIsReadyToHarvestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_HarvestCountIsReadyToHarvest, &state);
   }

   void HarvestCountIsReadyToHarvestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_HarvestCountIsReadyToHarvest, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(HarvestCountCalculator, ShouldSetHarvestCountIsReadyToHarvestWhenCalculationIsRequestedAndMinimumFreezeTimeIsMetAndIntegrationOfTemperatureReachesLimit)
{
   GivenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   GivenTheModuleIsInitialized();
   GivenTheHarvestCountCalculationRequestIs(SET);

   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN - MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   After(MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(SET);
}

TEST(HarvestCountCalculator, ShouldSetHarvestCountIsReadyToHarvestWhenCalculationIsRequestedAndMinimumFreezeTimeIsMetAfterIntegrationOfTemperatureReachesLimit)
{
   GivenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes - 1);
   GivenTheModuleIsInitialized();
   GivenTheHarvestCountCalculationRequestIs(SET);

   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN - MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   After(MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(SET);
}

TEST(HarvestCountCalculator, ShouldSetHarvestCountIsReadyToHarvestWhenCalculationIsRequestedBeforeInit)
{
   GivenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   GivenTheHarvestCountCalculationRequestIs(SET);
   GivenTheModuleIsInitialized();

   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN - MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   After(MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(SET);
}

TEST(HarvestCountCalculator, ShouldClearHarvestCountIsReadyToHarvestWhenCalculationIsNotRequested)
{
   GivenHarvestCountIsReadyToHarvestIs(SET);
   GivenTheModuleIsInitialized();
   GivenTheHarvestCountCalculationRequestIs(CLEAR);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);
}

TEST(HarvestCountCalculator, ShouldNotSetHarvestCountIsReadyToHarvestWhenCalculationIsNotRequestedAndMinimumFreezeTimeIsMetAndIntegrationOfTemperatureReachesLimit)
{
   GivenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   GivenTheModuleIsInitialized();
   GivenTheHarvestCountCalculationRequestIs(CLEAR);

   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);
}

TEST(HarvestCountCalculator, ShouldRestartHarvestCountWhenTemperatureGoesAboveStartIntegrationTemperature)
{
   GivenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   GivenTheModuleIsInitialized();
   GivenTheHarvestCountCalculationRequestIs(SET);

   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN - MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   WhenTheIceMakerTemperatureIs(aluminumMoldIceMakerData->freezeData.startIntegrationTemperatureInDegFx100 + 1);
   After(MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   WhenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   After(MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(SET);
}

TEST(HarvestCountCalculator, ShouldSetHarvestCountIsReadyToHarvestWhenCalculationIsRequestedAndThenTemperatureGoesBelowStartIntegrationTemperature)
{
   GivenTheIceMakerTemperatureIs(aluminumMoldIceMakerData->freezeData.startIntegrationTemperatureInDegFx100 + 1);
   GivenTheModuleIsInitialized();
   GivenTheHarvestCountCalculationRequestIs(SET);

   WhenTheIceMakerTemperatureIs(TemperatureThatCausesFreezeIntegrationLimitToBeReachedInMinimumFreezeTimeInMinutes);
   After(MSEC_PER_SEC);

   After(aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN - MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(CLEAR);

   After(MSEC_PER_SEC);
   HarvestCountIsReadyToHarvestShouldBe(SET);
}

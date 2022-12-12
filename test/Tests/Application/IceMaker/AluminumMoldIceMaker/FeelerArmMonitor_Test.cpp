/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeelerArmMonitor.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "SystemErds.h"
};

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define And
#define When

static const FeelerArmMonitorConfig_t config = {
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition
};

TEST_GROUP(FeelerArmMonitor)
{
   FeelerArmMonitor_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);
      aluminumMoldIceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;
   }

   void TheModuleIsInitialized()
   {
      FeelerArmMonitor_Init(&instance, dataModel, &config);
   }

   void FeelerArmMonitoringRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FeelerArmMonitoringRequest, &state);
   }

   void FeelerArmPositionIs(FeelerArmPosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerFeelerArmPosition, &position);
   }

   void FeelerArmIsReadyToEnterHarvestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FeelerArmIsReadyToEnterHarvest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void FeelerArmIsReadyToEnterHarvestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FeelerArmIsReadyToEnterHarvest, &state);
   }
};

TEST(FeelerArmMonitor, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(FeelerArmMonitor, ShouldSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhileFeelerArmMonitoringRequestedAndFeelerArmPositionIsBucketEmpty)
{
   Given TheModuleIsInitialized();

   When FeelerArmMonitoringRequestIs(SET);
   And FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);

   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN - 1);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   After(1);
   FeelerArmIsReadyToEnterHarvestShouldBe(SET);
}

TEST(FeelerArmMonitor, ShouldNotSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhileFeelerArmMonitorRequestedAndFeelerArmPositionIsBucketFull)
{
   Given TheModuleIsInitialized();

   When FeelerArmMonitoringRequestIs(SET);
   And FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhenFeelerArmPositionIsBucketFullWhileFeelerArmMonitoringRequested)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   When FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN - 1);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   After(1);
   FeelerArmIsReadyToEnterHarvestShouldBe(SET);
}

TEST(FeelerArmMonitor, ShouldNotSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhenFeelerArmMonitoringRequestIsClearWhileFeelerArmPositionIsBucketEmpty)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   When FeelerArmMonitoringRequestIs(CLEAR);
   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldNotSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhenFeelerArmMonitoringRequestIsSetWhileFeelerArmPositionIsBucketFull)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(CLEAR);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   When FeelerArmMonitoringRequestIs(SET);
   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldNotSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhenFeelerArmPositionIsBucketEmptyWhileFeelerArmMonitoringIsNotRequested)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(CLEAR);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);

   When FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldClearFeelerArmIsReadyToEnterHarvestWhenFeelerArmMonitoringRequestChangesToFalseWhileIceBucketIsEmpty)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   Given FeelerArmIsReadyToEnterHarvestIs(SET);

   When FeelerArmMonitoringRequestIs(CLEAR);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldClearFeelerArmIsReadyToEnterHarvestWhenFeelerArmMonitoringRequestedWhileIceBucketIsFull)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(CLEAR);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given FeelerArmIsReadyToEnterHarvestIs(SET);

   When FeelerArmMonitoringRequestIs(SET);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldClearFeelerArmIsReadyToEnterHarvestWhenIceBucketChangesToFullWhileFeelerArmMonitoringIsNotRequested)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(CLEAR);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   Given FeelerArmIsReadyToEnterHarvestIs(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldClearFeelerArmIsReadyToEnterHarvestWhenIceBucketChangesToFullWhileFeelerArmMonitoringIsRequested)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketEmpty);
   Given FeelerArmIsReadyToEnterHarvestIs(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

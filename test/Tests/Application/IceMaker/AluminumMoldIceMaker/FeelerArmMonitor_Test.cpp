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
   .feelerArmMonitoringRequestErd = Erd_IceMaker0_FeelerArmMonitoringRequest,
   .timerModuleErd = Erd_TimerModule,
   .feelerArmIsReadyToEnterHarvestErd = Erd_IceMaker0_FeelerArmIsReadyToEnterHarvest,
   .feelerArmPositionErd = Erd_IceMaker0_FeelerArmPosition
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
      aluminumMoldIceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot1Data->aluminumMoldData;
   }

   void TheModuleIsInitialized()
   {
      FeelerArmMonitor_Init(&instance, dataModel, &config, &aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes);
   }

   void FeelerArmMonitoringRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_FeelerArmMonitoringRequest, &state);
   }

   void FeelerArmPositionIs(FeelerArmPosition_t position)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_FeelerArmPosition, &position);
   }

   void FeelerArmIsReadyToEnterHarvestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_FeelerArmIsReadyToEnterHarvest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void FeelerArmIsReadyToEnterHarvestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_FeelerArmIsReadyToEnterHarvest, &state);
   }
};

TEST(FeelerArmMonitor, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(FeelerArmMonitor, ShouldSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhileFeelerArmMonitoringRequestedAndFeelerArmPositionIsBucketNotFull)
{
   Given TheModuleIsInitialized();

   When FeelerArmMonitoringRequestIs(SET);
   And FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);

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

   When FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN - 1);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);

   After(1);
   FeelerArmIsReadyToEnterHarvestShouldBe(SET);
}

TEST(FeelerArmMonitor, ShouldNotSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhenFeelerArmMonitoringRequestIsClearWhileFeelerArmPositionIsBucketNotFull)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
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

TEST(FeelerArmMonitor, ShouldNotSetFeelerArmIsReadyToEnterHarvestAfterFeelerArmDelayWhenFeelerArmPositionIsBucketNotFullWhileFeelerArmMonitoringIsNotRequested)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(CLEAR);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);

   When FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   After(aluminumMoldIceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldClearFeelerArmIsReadyToEnterHarvestWhenFeelerArmMonitoringRequestChangesToFalseWhileIceBucketIsNotFull)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
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
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given FeelerArmIsReadyToEnterHarvestIs(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

TEST(FeelerArmMonitor, ShouldClearFeelerArmIsReadyToEnterHarvestWhenIceBucketChangesToFullWhileFeelerArmMonitoringIsRequested)
{
   Given TheModuleIsInitialized();
   Given FeelerArmMonitoringRequestIs(SET);
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given FeelerArmIsReadyToEnterHarvestIs(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmIsReadyToEnterHarvestShouldBe(CLEAR);
}

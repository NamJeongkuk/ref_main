/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DamperMaxOpenTimeMonitor.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DamperData.h"
#include "TddPersonality.h"
#include "DamperVotedPosition.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When
#define Then
#define And

static const DamperMaxOpenTimeConfiguration_t config = {
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .damperFullCycleRequestErd = Erd_DamperFullCycleRequest
};

TEST_GROUP(DamperMaxOpenTime)
{
   DamperMaxOpenTime_t instance;
   I_DataModel_t *dataModel;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DamperData_t *damperData;
   const DamperPosition_t *damperCurrentPosition;
   const DamperPosition_t *damperPosition;

   void setup()
   {
      damperPosition = (const DamperPosition_t *)DamperPosition_Closed;

      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      damperData = PersonalityParametricData_Get(dataModel)->damperData;
      damperCurrentPosition = damperPosition;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, 1000);
   }

   void DamperCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(dataModel, Erd_DamperCurrentPosition, &position);
   }

   void TheModuleIsInitialized()
   {
      DamperMaxOpenTimeMonitor_Init(&instance, dataModelDouble.dataModel, &config, damperData);
   }

   void GivenFullCycleRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DamperFullCycleRequest, &state);
   }

   void FullCycleRequestShouldBeClear()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_FALSE(actualRequest);
   }

   void FullCycleRequestShouldBeSet()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_TRUE(actualRequest);
   }
};

TEST(DamperMaxOpenTime, ShouldStartWithOpenDamperThenTimerOutAndWriteDamperCloseAndCareToTrueMaxOpenVote)
{
   Given DamperCurrentPositionIs(DamperPosition_Open);
   Given TheModuleIsInitialized();

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(DamperMaxOpenTime, ShouldStartDamperMaxOpenTimerWhenDamperOpensThenStartFullCycle)
{
   Given TheModuleIsInitialized();
   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeSet();
}

TEST(DamperMaxOpenTime, ShouldStartDamperMaxOpenTimerWhenDamperOpensThenStopTimerWhenDamperIsClosedAndNotStartFullCycle)
{
   Given TheModuleIsInitialized();
   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);

   When DamperCurrentPositionIs(DamperPosition_Closed);

   After(1);
   FullCycleRequestShouldBeClear();
}

TEST(DamperMaxOpenTime, ShouldStartTimerWhenDamperOpensThenStopTimerAndNotStartFullCycleWhenDamperIsClosedThenStartTimerAgainWhenDamperOpensThenStartFullCycleWhenTimerExpires)
{
   Given TheModuleIsInitialized();
   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);

   When DamperCurrentPositionIs(DamperPosition_Closed);

   FullCycleRequestShouldBeClear();

   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);

   FullCycleRequestShouldBeSet();
}

TEST_GROUP(DamperMaxOpenTime_Zero)
{
   DamperMaxOpenTime_t instance;
   I_DataModel_t *dataModel;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   const DamperData_t *damperData;
   const DamperPosition_t *damperCurrentPosition;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentMaxOpenDamperTimerZero);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      damperData = PersonalityParametricData_Get(dataModel)->damperData;
      damperCurrentPosition = (const DamperPosition_t *)DamperPosition_Closed;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void TheModuleIsInitialized()
   {
      DamperMaxOpenTimeMonitor_Init(&instance, dataModelDouble.dataModel, &config, damperData);
   }

   void DamperCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(dataModel, Erd_DamperCurrentPosition, &position);
   }

   void FullCycleRequestShouldBeClear()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_FALSE(actualRequest);
   }
};

TEST(DamperMaxOpenTime_Zero, ShouldNotRunTestIfMaxTimeForDamperToBeOpenInMinutesIsZero)
{
   Given TheModuleIsInitialized();

   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);

   FullCycleRequestShouldBeClear();
}

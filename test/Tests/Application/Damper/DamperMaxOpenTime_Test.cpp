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
   .damperPositionMaxOpenTimeVoteErd = Erd_FreshFoodDamperPosition_MaxOpenTimeVote,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition
};

TEST_GROUP(MaxOpenTime)
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

   void DamperCareStateShouldBeDontCare()
   {
      DamperVotedPosition_t actualPosition;

      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_MaxOpenTimeVote, &actualPosition);
      CHECK_EQUAL(actualPosition.care, Vote_DontCare);
   }

   void DamperCareStateShouldBeDamperClosedAndCareTrue()
   {
      DamperVotedPosition_t actualPosition;

      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_MaxOpenTimeVote, &actualPosition);
      CHECK_EQUAL(actualPosition.care, Vote_Care);
      CHECK_EQUAL(actualPosition.position, DamperPosition_Closed);
   }
};

TEST(MaxOpenTime, ShouldStartWithOpenDamperThenTimerOutAndWriteDamperCloseAndCareToTrueMaxOpenVote)
{
   Given DamperCurrentPositionIs(DamperPosition_Open);
   Given TheModuleIsInitialized();

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   DamperCareStateShouldBeDontCare();

   After(1);
   DamperCareStateShouldBeDamperClosedAndCareTrue();
}

TEST(MaxOpenTime, ShouldStartMaxOpenTimerWhenDamperOpensThenSendMaxVoteRequestErdWithDamperCloseAndCareIsTrue)
{
   Given TheModuleIsInitialized();
   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);
   DamperCareStateShouldBeDontCare();

   After(1);
   DamperCareStateShouldBeDamperClosedAndCareTrue();
}

TEST(MaxOpenTime, ShouldStartMaxOpenTimerWhenDamperIsOpenThenStopTimerWhenDamperIsClosedAndSetMaxOpenVoteErdCareToFalse)
{
   Given TheModuleIsInitialized();
   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);

   When DamperCurrentPositionIs(DamperPosition_Closed);

   DamperCareStateShouldBeDontCare();
}

TEST(MaxOpenTime, ShouldStartTimerWhenDamperOpensThenStopsTimerAndSetCareToFalseWhenDamperIsClosedThenStartTimerAgainWhenDamperOpensThenRequestDamperCloseWhenTimerExpires)
{
   Given TheModuleIsInitialized();
   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN - 1);

   When DamperCurrentPositionIs(DamperPosition_Closed);

   DamperCareStateShouldBeDontCare();

   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);

   DamperCareStateShouldBeDamperClosedAndCareTrue();
}

TEST_GROUP(MaxOpenTime_Zero)
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
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, 1000);
   }

   void TheModuleIsInitialized()
   {
      DamperMaxOpenTimeMonitor_Init(&instance, dataModelDouble.dataModel, &config, damperData);
   }

   void DamperCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(dataModel, Erd_DamperCurrentPosition, &position);
   }

   void DamperCareStateShouldBeDontCare()
   {
      DamperVotedPosition_t actualPosition;

      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_MaxOpenTimeVote, &actualPosition);
      CHECK_EQUAL(actualPosition.care, Vote_DontCare);
   }
};

TEST(MaxOpenTime_Zero, ShouldNotRunTestIfMaxTimeForDamperToBeOpenInMinutesIsZero)
{
   Given TheModuleIsInitialized();

   When DamperCurrentPositionIs(DamperPosition_Open);

   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);

   DamperCareStateShouldBeDontCare();
}

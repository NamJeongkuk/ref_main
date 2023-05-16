/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "UiHeartbeatTickMonitor.h"
#include "Signal.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   MaxUiUpdateIntervalInMsec = 1500,
   ALongTimeInMsec = 10 * MaxUiUpdateIntervalInMsec
};

static const UiHeartbeatTickMonitorConfiguration_t config = {
   .heartbeatTickFromUiErd = Erd_AndroidUiSignOfLife,
   .activeCommunicationWithUiErd = Erd_MainboardIsCommunicatingToAndroidUi,
   .maxUiUpdateIntervalInMsec = MaxUiUpdateIntervalInMsec
};

TEST_GROUP(UiHeartbeatTickMonitor)
{
   UiHeartbeatTickMonitor_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   I_DataModel_t *dataModel;
   EventSubscription_t dataModelOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      EventSubscription_Init(&dataModelOnChangeSubscription, dataModel, DataModelChanged);
      DataModel_SubscribeAll(dataModel, &dataModelOnChangeSubscription);
   }

   static void DataModelChanged(void *context, const void *args)
   {
      const DataModelOnDataChangeArgs_t *onChangeData = (const DataModelOnDataChangeArgs_t *)args;

      if(onChangeData->erd == Erd_MainboardIsCommunicatingToAndroidUi)
      {
         const bool *data = (const bool *)onChangeData->data;
         mock()
            .actualCall("Mainboard Has Communicated With UI")
            .onObject(context)
            .withParameter("Data", *data);
      }
   }

   void ExpectThatTheMainboardHasCommunicationWithUiErdChangesTo(bool expected)
   {
      mock()
         .expectOneCall("Mainboard Has Communicated With UI")
         .onObject(dataModel)
         .withParameter("Data", expected);
   }

   void ExpectMainboardHasCommunicationWithUiErdNotToChange()
   {
      mock().expectNoCall("Mainboard Has Communicated With UI");
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheModuleIsInitialized()
   {
      UiHeartbeatTickMonitor_Init(
         &instance,
         &config,
         dataModel);
   }

   void WhenTheUiSendsAHeartbeatTick()
   {
      Signal_SendViaErd(
         DataModel_AsDataSource(dataModel),
         Erd_AndroidUiSignOfLife);
   }

   void GivenTheModuleIsInitializedAndCommunicationIsActive()
   {
      UiHeartbeatTickMonitor_Init(
         &instance,
         &config,
         dataModel);
      ExpectThatTheMainboardHasCommunicationWithUiErdChangesTo(SET);
      WhenTheUiSendsAHeartbeatTick();
   }

   void TheMainboardShouldNotHaveCommunicationWithTheUi()
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_MainboardIsCommunicatingToAndroidUi,
         &actual);

      CHECK_FALSE(actual);
   }

   void TheMainboardShouldHaveCommunicationWithTheUi()
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_MainboardIsCommunicatingToAndroidUi,
         &actual);

      CHECK_TRUE(actual);
   }
};

TEST(UiHeartbeatTickMonitor, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(UiHeartbeatTickMonitor, ShouldNotHaveCommunicationWithOnInit)
{
   GivenTheModuleIsInitialized();
   TheMainboardShouldNotHaveCommunicationWithTheUi();
}

TEST(UiHeartbeatTickMonitor, ShouldHaveCommunicationWithWhenSignOfLifeIsSent)
{
   GivenTheModuleIsInitialized();

   ExpectThatTheMainboardHasCommunicationWithUiErdChangesTo(SET);
   WhenTheUiSendsAHeartbeatTick();
}

TEST(UiHeartbeatTickMonitor, ShouldLoseCommunicationWithWhenSignOfLifeIsNotSentForUpdateInterval)
{
   GivenTheModuleIsInitializedAndCommunicationIsActive();
   After(MaxUiUpdateIntervalInMsec - 1);
   TheMainboardShouldHaveCommunicationWithTheUi();

   ExpectThatTheMainboardHasCommunicationWithUiErdChangesTo(CLEAR);
   After(1);
}

TEST(UiHeartbeatTickMonitor, ShouldNotLoseCommunicationWithWhenSignOfLifeIsContinuouslySetBeforeUpdateIntervalExpires)
{
   GivenTheModuleIsInitializedAndCommunicationIsActive();

   ExpectMainboardHasCommunicationWithUiErdNotToChange();
   for(uint8_t i = 0; i < 10; i++)
   {
      After(MaxUiUpdateIntervalInMsec - 1);
      WhenTheUiSendsAHeartbeatTick();
   }
}

TEST(UiHeartbeatTickMonitor, ShouldRegainLostCommunicationWithWhenSignOfLifeIsSentAfterALongInterval)
{
   GivenTheModuleIsInitializedAndCommunicationIsActive();

   ExpectThatTheMainboardHasCommunicationWithUiErdChangesTo(CLEAR);
   After(MaxUiUpdateIntervalInMsec);

   ExpectMainboardHasCommunicationWithUiErdNotToChange();
   After(ALongTimeInMsec);

   ExpectThatTheMainboardHasCommunicationWithUiErdChangesTo(SET);
   WhenTheUiSendsAHeartbeatTick();
}

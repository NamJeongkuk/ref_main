/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AndroidUiSignOfLifeMonitor.h"
#include "Signal.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"
#include "AndroidUiSignOfLifeFsmState.h"
#include "Constants_Time.h"

enum
{
   MaxUiUpdateIntervalInMinutes = 2,
   MaxTimeBeforeUiResetAttemptInMinutes = 10,
   AndroidUiCommunicationFaultTimeInMinutes = 15
};

static const AndroidUiSignOfLifeMonitorConfiguration_t config = {
   .heartbeatTickFromUiErd = Erd_PrimaryUiSignOfLife,
   .uiSignOfLifeFsmStateErd = Erd_AndroidUiSignOfLifeFsmState,
   .mainboardIsConnectedToUiNvErd = Erd_MainboardIsConnectedToTheAndroidUi,
   .numberOfTimesMainboardHasLostCommunicationWithUi = Erd_NumberOfTimesMainboardHasLostCommunicationWithAndroidUi,
   .numberOfTimesMainboardHasAttemptedToResetUi = Erd_NumberOfTimesMainboardHasAttemptedToResetAndroidUi,
   .uiResetSignalErd = Erd_AndroidUiResetSignal,
   .maxUiUpdateIntervalInMinutes = MaxUiUpdateIntervalInMinutes,
   .resetTimeInMinutes = MaxTimeBeforeUiResetAttemptInMinutes,
   .faultTimeInMinutes = AndroidUiCommunicationFaultTimeInMinutes
};

TEST_GROUP(AndroidUiSignOfLifeMonitor)
{
   AndroidUiSignOfLifeMonitor_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   I_DataModel_t *dataModel;

   EventSubscription_t uiResetEventSubscription;

   static void UiResetChanged(void *context, const void *args)
   {
      IGNORE(args);

      mock().actualCall("Ui Reset Event Subscription Changed").onObject(context);
   }

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      EventSubscription_Init(&uiResetEventSubscription, dataModel, UiResetChanged);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void WhenTheSignOfLifeSignalIsSent()
   {
      Signal_SendViaErd(
         DataModel_AsDataSource(dataModel),
         Erd_PrimaryUiSignOfLife);
   }

   void GivenTheMainboardConnectedErdIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_MainboardIsConnectedToTheAndroidUi,
         &state);
   }

   void WhenTheMainboardConnectedErdIs(bool state)
   {
      GivenTheMainboardConnectedErdIs(state);
   }

   void GivenTheModuleIsInitialized()
   {
      AndroidUiSignOfLifeMonitor_Init(
         &instance,
         &config,
         dataModel);
   }

   void TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_t expected)
   {
      AndroidUiSignOfLifeFsmState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AndroidUiSignOfLifeFsmState,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_t expectedState)
   {
      GivenTheModuleIsInitialized();
      if(expectedState == AndroidUiSignOfLifeFsmState_Fault)
      {
         After(AndroidUiCommunicationFaultTimeInMinutes * MSEC_PER_MIN);
      }
      else if(expectedState == AndroidUiSignOfLifeFsmState_LostCommunicationWithUi)
      {
         After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN);
      }
      else if(expectedState == AndroidUiSignOfLifeFsmState_HasCommunicationWithUi)
      {
         WhenTheSignOfLifeSignalIsSent();
      }

      TheAndroidUiSignOfLifeFsmStateShouldBe(expectedState);
   }

   void TheNumberOfTimesTheMainboardHasLostCommunicationShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_NumberOfTimesMainboardHasLostCommunicationWithAndroidUi,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheNumberOfResetAttemptsShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_NumberOfTimesMainboardHasAttemptedToResetAndroidUi,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ExpectUiResetSignalToBeSent()
   {
      DataModel_Subscribe(dataModel, Erd_AndroidUiResetSignal, &uiResetEventSubscription);
      mock()
         .expectOneCall("Ui Reset Event Subscription Changed")
         .onObject(dataModel);
   }
};

TEST(AndroidUiSignOfLifeMonitor, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(AndroidUiSignOfLifeMonitor, InitialHsmStateShouldBeIdleWhenInitializedWithUnconnectedUi)
{
   GivenTheMainboardConnectedErdIs(CLEAR);
   GivenTheModuleIsInitialized();
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_Idle);
}

TEST(AndroidUiSignOfLifeMonitor, StateShouldTransitionFromLostCommunicationToHasCommunicationWhenSignOfLifeChanges)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);

   WhenTheSignOfLifeSignalIsSent();
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, StateShouldTransitionFromFaultToHasCommunicationWhenSignOfLifeChanges)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Fault);

   WhenTheSignOfLifeSignalIsSent();
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, StateShouldTransitionFromHasCommunicationToLostCommunicationWhenSignOfLifeDoesNotChangeForMaxUiUpdateInterval)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN - 1);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   After(1);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, StateShouldTransitionFromLostCommunicationToFaultWhenSignOfLifeDoesNotChangeForMaxTimeBeforeUiResetAttempt)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   After(AndroidUiCommunicationFaultTimeInMinutes * MSEC_PER_MIN - 1);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);

   After(1);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_Fault);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldIncrementNumberTimesMainboardHasLostCommunicationWithAndroidUi)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   for(uint8_t i = 0; i < 10; i++)
   {
      WhenTheSignOfLifeSignalIsSent();
      TheNumberOfTimesTheMainboardHasLostCommunicationShouldBe(i);
      After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN);
   }
}

TEST(AndroidUiSignOfLifeMonitor, ShouldIncrementRestAttemptsAfterMaxResetTime)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   for(uint8_t i = 0; i < 10; i++)
   {
      WhenTheSignOfLifeSignalIsSent();
      TheNumberOfResetAttemptsShouldBe(i);
      After(MaxTimeBeforeUiResetAttemptInMinutes * MSEC_PER_MIN);
   }
}

TEST(AndroidUiSignOfLifeMonitor, ShouldSendUiResetSignalAfterResetTime)
{
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   ExpectUiResetSignalToBeSent();
   After(MaxTimeBeforeUiResetAttemptInMinutes * MSEC_PER_MIN);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldTransitionToFaultStateAfterFaultTime)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   After(AndroidUiCommunicationFaultTimeInMinutes * MSEC_PER_MIN);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_Fault);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldRemainInHasCommunicationStateIfSignOfLifeIsUpdatedWithinMaxUiUpdateTime)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN - 1);

   WhenTheSignOfLifeSignalIsSent();
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);

   for(uint8_t i = 0; i < 10; i++)
   {
      After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN - 1);

      WhenTheSignOfLifeSignalIsSent();
      TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);
   }
}

TEST(AndroidUiSignOfLifeMonitor, ShouldTransitionFromIdleStateToLostCommunicationWhenAndroidUiBecomesConnectedAndMaxUpdateIntervalElapses)
{
   WhenTheMainboardConnectedErdIs(CLEAR);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Idle);

   WhenTheMainboardConnectedErdIs(SET);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_Idle);

   After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldTransitionFromIdleStateToLostCommunicationAfterMaxUpdateIntervalElapsesWhenMainboardIsConnectedErd)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Idle);

   After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldTransitionFromIdleStateToHasCommunicationWhenSignOfLifeSignalIsSent)
{
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Idle);

   WhenTheSignOfLifeSignalIsSent();
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_HasCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldTransitionFromIdleStateToLostCommunicationWhenSignOfLifeSignalIsNotSentForMaxUiUpdateIntervalInMinutes)
{
   WhenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Idle);

   After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);
}

TEST(AndroidUiSignOfLifeMonitor, ShouldRemainInIdleStateIfInitializedWithoutAndroidUiBeingConnected)
{
   GivenTheMainboardConnectedErdIs(CLEAR);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Idle);

   After(AndroidUiCommunicationFaultTimeInMinutes * 10 * MSEC_PER_MIN);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_Idle);
}

TEST(AndroidUiSignOfLifeMonitor, StateShouldTransitionFromIdleToLostCommunicationAfterMaxUpdateIntervalWhenMainboardIsConnectedToUi)
{
   GivenTheMainboardConnectedErdIs(SET);
   GivenTheModuleIsInitializedAndEntersState(AndroidUiSignOfLifeFsmState_Idle);

   After(MaxUiUpdateIntervalInMinutes * MSEC_PER_MIN);
   TheAndroidUiSignOfLifeFsmStateShouldBe(AndroidUiSignOfLifeFsmState_LostCommunicationWithUi);
}

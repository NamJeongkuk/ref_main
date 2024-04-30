/*!
 * @file
 * @brief Integration test for Three Way Sealed System Valve Plugin
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "EventQueueInterruptSafePlugin.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "CppUTestExt/MockSupport.h"
#include "TddPersonality.h"

enum
{
   OneStep = 1
};

TEST_GROUP(ThreeWaySealedSystemValvePlugin)
{
   Application_t instance;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleDouble;
   Interrupt_TestDouble_t *interruptTestDouble;
   const SealedSystemValveData_t *valveData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleDouble->timerModule);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt);
      valveData = PersonalityParametricData_Get(dataModel)->sealedSystemValveData;
   }

   void GivenTheApplicationIsInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks);
      AfterNInterrupts(ticks);
      EventQueue_InterruptSafe_Run(EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue());
   }

   void SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_t expected)
   {
      SealedSystemValvePosition_t actual;
      DataModel_Read(dataModel, Erd_SealedSystemValveCurrentPosition, &actual);
      CHECK_EQUAL(expected, actual);
   };
};

TEST(ThreeWaySealedSystemValvePlugin, ShouldSetSealedSystemPositionToBOnApplicationInit)
{
   GivenTheApplicationIsInitialized();
   After((valveData->refrigerantValvePowerUpTimeInSeconds) * MSEC_PER_SEC);
   SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_Home);

   After((2 * valveData->excitationDelayInMilliseconds) + (valveData->positionBStep + OneStep) * (valveData->delayBetweenStepEventsInMilliseconds) - 1);
   SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_Home);

   After(1);
   SealedSystemValveCurrentPositionShouldBe(SealedSystemValvePosition_B);
}

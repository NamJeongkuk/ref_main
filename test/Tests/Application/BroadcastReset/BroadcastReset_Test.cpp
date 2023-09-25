/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "BroadcastReset.h"
#include "Gea2CommonCommands.h"
#include "Gea2Addresses.h"
#include "DataModelErdPointerAccess.h"
#include "DataModel.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "Gea2MessageEndpoint_TestDouble.h"
#include "ReferDataModel_TestDouble.h"
#include "Action_Mock.h"
#include "TimeSource_TestDouble.h"
#include "Constants_Time.h"
#include "TddPersonality.h"

enum
{
   NumberOfGea2MessageRetries = 3,
   ResetBoardCommand = 0xDD,
   ResetBoardData = 0xF401,
   FiveSecondsInMsec = 5 * MSEC_PER_SEC,
   DefaultPersonalityForTest = TddPersonality_DevelopmentSingleEvaporator
};

static const BroadcastResetConfig_t config = {
   .broadcastResetRequestErd = Erd_BroadcastResetRequestSignal,
   .resetErd = Erd_Reset,
   .gea2MessageEndpointErd = Erd_Gea2MessageEndpoint
};

STATIC_ALLOC_GEA2MESSAGE(expectedBroadcastResetMessage, 2);

TEST_GROUP(BroadcastReset)
{
   BroadcastReset_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   Gea2MessageEndpoint_TestDouble_t messageEndpointTestDouble;
   I_Gea2MessageEndpoint_t *messageEndpoint;

   Action_Mock_t actionMock;
   I_Action_t *resetAction;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      Action_Mock_Init(&actionMock);
      resetAction = &actionMock.interface;

      ReferDataModel_TestDouble_Init(&referDataModelTestDouble, resetAction, DefaultPersonalityForTest);
      dataModel = referDataModelTestDouble.dataModel;

      Gea2MessageEndpoint_TestDouble_Init(
         &messageEndpointTestDouble,
         Gea2Address_Mainboard);

      messageEndpoint = &messageEndpointTestDouble.interface;
      timerModuleTestDouble = (TimerModule_TestDouble_t *)DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);

      DataModel_Write(
         dataModel,
         Erd_Gea2MessageEndpoint,
         &messageEndpoint);
   }

   void GivenTheBroadcastResetIsInitialized()
   {
      BroadcastReset_Init(
         &instance,
         dataModel,
         &config);
   }

   void SendBroadcastResetRequestSignal()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_BroadcastResetRequestSignal);
   }

   void Gea2MessageShouldBeRequestedForBroadcastReset()
   {
      Gea2Message_SetDestination(expectedBroadcastResetMessage, Gea2Address_AllBoards);
      Gea2Message_SetCommand(expectedBroadcastResetMessage, ResetBoardCommand);
      Gea2Message_SetSource(expectedBroadcastResetMessage, Gea2Address_Mainboard);
      uint8_t *payload = Gea2Message_GetPayload(expectedBroadcastResetMessage);
      payload[0] = ResetBoardData >> 8;
      payload[1] = ResetBoardData & 0xFF;

      mock()
         .expectOneCall("Send")
         .onObject(messageEndpoint)
         .withParameterOfType("Gea2Message_t", "message", expectedBroadcastResetMessage)
         .withParameter("retries", NumberOfGea2MessageRetries);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void ResetActionShouldBeInvoked()
   {
      mock().expectOneCall("Invoke").onObject(resetAction);
   }

   void WhenResetIsRequested()
   {
      SendBroadcastResetRequestSignal();
   }

   void NothingShouldHappen()
   {
      mock().expectNoCall("none");
   }
};

TEST(BroadcastReset, ShouldSendGea2MessageToResetAllBoardsWhenResetIsRequested)
{
   GivenTheBroadcastResetIsInitialized();

   Gea2MessageShouldBeRequestedForBroadcastReset();
   WhenResetIsRequested();
}

TEST(BroadcastReset, ShouldResetSelfAfterDelay)
{
   GivenTheBroadcastResetIsInitialized();

   Gea2MessageShouldBeRequestedForBroadcastReset();
   WhenResetIsRequested();

   NothingShouldHappen();
   After(FiveSecondsInMsec - 1);

   ResetActionShouldBeInvoked();
   After(1);
}

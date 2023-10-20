/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostTestRequestHandler.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define When

static const DefrostTestRequestHandlerConfiguration_t config = {
   .defrostStateErd = Erd_DefrostState,
   .defrostTestRequestErd = Erd_DefrostTestRequest,
   .disableDefrostErd = Erd_DisableDefrost,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .nextDefrostTypeErd = Erd_NextDefrostType,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .defrostTestRequestStatusErd = Erd_DefrostTestRequestStatus,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill
};

TEST_GROUP(DefrostTestRequestHandler)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   DefrostTestRequestHandler_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void GivenDefrostTestRequestHandlerIsInitialized()
   {
      DefrostTestRequestHandler_Init(&instance, dataModel, &config);
   }

   void WhenDefrostTestRequestIs(DefrostTestRequest_t request)
   {
      DataModel_Write(dataModel, Erd_DefrostTestRequest, &request);
   }

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void DefrostTestStateRequestShouldBe(DefrostTestStateRequest_t expectedRequest, Signal_t expectedRequestId)
   {
      DefrostTestStateRequestMessage_t actual;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &actual);

      CHECK_EQUAL(expectedRequest, actual.request);
      CHECK_EQUAL(expectedRequestId, actual.requestId);
   }

   void DefrostTestStateRequestShouldNotBeRequested()
   {
      DefrostTestStateRequestMessage_t actual;
      DataModel_Read(dataModel, Erd_DefrostTestStateRequest, &actual);

      CHECK_EQUAL(0, actual.requestId);
   }

   void DefrostTestRequestStatusShouldBe(DefrostTestStateRequest_t expected)
   {
      DefrostTestStateRequest_t actual;
      DataModel_Read(dataModel, Erd_DefrostTestRequestStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DefrostTestStateRequestIsInitialized()
   {
      DefrostTestStateRequestMessage_t message;
      message.request = DefrostTestStateRequest_Idle;
      message.requestId = 0;
      DataModel_Write(dataModel, Erd_DefrostTestStateRequest, &message);
   }

   void DisableDefrostShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DisableDefrost, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DisableDefrostIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DisableDefrost, &state);
   }

   void NextDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostType, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void DefrostTestRequestShouldBeReset()
   {
      DefrostTestRequest_t actual;
      DataModel_Read(dataModel, Erd_DefrostTestRequest, &actual);

      CHECK_EQUAL(DefrostTestRequest_None, actual);
   }

   void DontSkipPrechillShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DontSkipDefrostPrechill, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsPrechill)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Prechill);

   WhenDefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsHeaterOn)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_HeaterOn);

   WhenDefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsDwell)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Dwell);

   WhenDefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsPostDwell)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_PostDwell);

   WhenDefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldNotRequestWhenDefrostTestRequestIsIdleWhileDefrostStateIsDisabled)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Disabled);

   WhenDefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 0);
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsPrechill)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Prechill);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsHeaterOn)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_HeaterOn);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsDwell)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Dwell);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsPostDwell)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_PostDwell);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldNotRequestWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsDisabled)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Disabled);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 0);
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFullAndRequestDefrostWhenDefrostTestRequestIsDefrostWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_Defrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Defrost);
   NextDefrostTypeShouldBe(DefrostType_Full);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestPrechillWhenDefrostTestRequestIsFreshFoodOnlyPrechillWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyPrechill);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Prechill, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFullAndRequestPrechillWhenDefrostTestRequestIsPrechillWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_Prechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Prechill);
   NextDefrostTypeShouldBe(DefrostType_Full);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Prechill, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndDoTheOthersWhenDefrostTestRequestIsAhamFreshFoodOnlyPrechillWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_AhamFreshFoodOnlyPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_AhamFreshFoodOnlyPrechill);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DefrostTestRequestShouldBeReset();
   DontSkipPrechillShouldBe(SET);
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFullAndDoTheOthersWhenDefrostTestRequestIsAhamPrechillWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_AhamPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_AhamPrechill);
   NextDefrostTypeShouldBe(DefrostType_Full);
   UseAhamPrechillReadyToDefrostTimeAndResetDefrostCountsShouldBe(true);
   DefrostTestRequestShouldBeReset();
   DontSkipPrechillShouldBe(SET);
}

TEST(DefrostTestRequestHandler, ShouldSetDisableDefrostToTrueWhenDefrostTestRequestIsDisableWhileDefrostStateIsIdle)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_Disable);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Disable);
   DisableDefrostShouldBe(true);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetDisableDefrostToFalseWhenDefrostTestRequestIsEnableWhileDefrostStateIsDisabled)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Disabled);

   WhenDefrostTestRequestIs(DefrostTestRequest_Enable);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Enable);
   DisableDefrostShouldBe(false);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetDisableDefrostToTrueWhenDefrostTestRequestIsDisableWhileDefrostStateIsDisabled)
{
   GivenDefrostStateIs(DefrostState_Disabled);
   GivenDefrostTestRequestHandlerIsInitialized();

   WhenDefrostTestRequestIs(DefrostTestRequest_Disable);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Disable);
   DisableDefrostShouldBe(true);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestExitDefrostHeaterOnStateAndSetDefrostTestRequestStateToExitDefrostHeaterOnStateWhenDefrostTestRequestIsExitDefrostHeaterOnState)
{
   GivenDefrostTestRequestHandlerIsInitialized();
   GivenDefrostStateIs(DefrostState_Idle);

   WhenDefrostTestRequestIs(DefrostTestRequest_ExitDefrostHeaterOnState);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_ExitDefrostHeaterOnState);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_ExitDefrostHeaterOnState, 1);
   DefrostTestRequestShouldBeReset();
}

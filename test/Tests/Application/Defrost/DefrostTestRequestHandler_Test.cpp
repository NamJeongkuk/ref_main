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
   .useMinimumReadyToDefrostTimeErd = Erd_UseMinimumReadyToDefrostTime,
   .resetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestErd = Erd_ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal,
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

   void DefrostTestRequestHandlerIsInitialized()
   {
      DefrostTestRequestHandler_Init(&instance, dataModel, &config);
   }

   void DefrostTestRequestIs(DefrostTestRequest_t request)
   {
      DataModel_Write(dataModel, Erd_DefrostTestRequest, &request);
   }

   void DefrostStateIs(DefrostState_t state)
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

   void UseMinimumReadyToDefrostTimeShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_UseMinimumReadyToDefrostTime, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestShouldBeIncremented()
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal, &actual);

      CHECK_EQUAL(1, actual);
   }

   void ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestShouldBeIncrementedAgain()
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestSignal, &actual);

      CHECK_EQUAL(2, actual);
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
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsPrechill)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Prechill);

   When DefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsHeaterOn)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_HeaterOn);

   When DefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsDwell)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Dwell);

   When DefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldRequestIdleWhenDefrostTestRequestIsIdleWhileDefrostStateIsPostDwell)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_PostDwell);

   When DefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldNotRequestWhenDefrostTestRequestIsIdleWhileDefrostStateIsDisabled)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Disabled);

   When DefrostTestRequestIs(DefrostTestRequest_Idle);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 0);
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsPrechill)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Prechill);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsHeaterOn)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_HeaterOn);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsDwell)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Dwell);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestDefrostWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsPostDwell)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_PostDwell);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyDefrost);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldNotRequestWhenDefrostTestRequestIsFreshFoodOnlyDefrostWhileDefrostStateIsDisabled)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Disabled);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyDefrost);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Idle, 0);
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFullAndRequestDefrostWhenDefrostTestRequestIsDefrostWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_Defrost);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Defrost);
   NextDefrostTypeShouldBe(DefrostType_Full);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Defrost, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndRequestPrechillWhenDefrostTestRequestIsFreshFoodOnlyPrechillWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_FreshFoodOnlyPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_FreshFoodOnlyPrechill);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Prechill, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFullAndRequestPrechillWhenDefrostTestRequestIsPrechillWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_Prechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Prechill);
   NextDefrostTypeShouldBe(DefrostType_Full);
   DefrostTestStateRequestShouldBe(DefrostTestStateRequest_Prechill, 1);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFreshFoodAndDoTheOthersWhenDefrostTestRequestIsAhamFreshFoodOnlyPrechillWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_AhamFreshFoodOnlyPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_AhamFreshFoodOnlyPrechill);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestShouldBeIncremented();
   DefrostTestRequestShouldBeReset();
   DontSkipPrechillShouldBe(SET);
}

TEST(DefrostTestRequestHandler, ShouldSetNextDefrostTypeToFullAndDoTheOthersWhenDefrostTestRequestIsAhamPrechillWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_AhamPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_AhamPrechill);
   NextDefrostTypeShouldBe(DefrostType_Full);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestShouldBeIncremented();
   DefrostTestRequestShouldBeReset();
   DontSkipPrechillShouldBe(SET);
}

TEST(DefrostTestRequestHandler, ShouldIncrementResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestWheneverDefrostTestRequestIsAhamFreshFoodOnlyPrechillOrAhamPrechill)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_AhamFreshFoodOnlyPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_AhamFreshFoodOnlyPrechill);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestShouldBeIncremented();
   DefrostTestRequestShouldBeReset();

   When DefrostTestRequestIs(DefrostTestRequest_AhamPrechill);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_AhamPrechill);
   NextDefrostTypeShouldBe(DefrostType_Full);
   UseMinimumReadyToDefrostTimeShouldBe(true);
   ResetAndCountDefrostCompressorOnTimeCountsAndDoorAccelerationsRequestShouldBeIncrementedAgain();
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetDisableDefrostToTrueWhenDefrostTestRequestIsDisableWhileDefrostStateIsIdle)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Idle);

   When DefrostTestRequestIs(DefrostTestRequest_Disable);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Disable);
   DisableDefrostShouldBe(true);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetDisableDefrostToFalseWhenDefrostTestRequestIsEnableWhileDefrostStateIsDisabled)
{
   Given DefrostTestRequestHandlerIsInitialized();
   Given DefrostStateIs(DefrostState_Disabled);

   When DefrostTestRequestIs(DefrostTestRequest_Enable);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Enable);
   DisableDefrostShouldBe(false);
   DefrostTestRequestShouldBeReset();
}

TEST(DefrostTestRequestHandler, ShouldSetDisableDefrostToTrueWhenDefrostTestRequestIsDisableWhileDefrostStateIsDisabled)
{
   Given DefrostStateIs(DefrostState_Disabled);
   Given DefrostTestRequestHandlerIsInitialized();

   When DefrostTestRequestIs(DefrostTestRequest_Disable);
   DefrostTestRequestStatusShouldBe(DefrostTestRequest_Disable);
   DisableDefrostShouldBe(true);
   DefrostTestRequestShouldBeReset();
}

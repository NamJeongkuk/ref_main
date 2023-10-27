/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ServiceModeRequestHandler.h"
#include "SystemErds.h"
#include "ServiceModeTestRequest.h"
#include "ServiceModeTestNumber.h"
#include "ServiceModeTestStatus.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "ServiceTest_Mock.h"

enum
{
   ValidTestNumber0,
   ValidTestNumber1,
   NonInitializedTestNumber,
   NumberOfServiceTests,
   OutOfBoundTestNumber = NumberOfServiceTests,
};

TEST_GROUP(ServiceModeRequestHandler)
{
   ServiceModeRequestHandler_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   ServiceTest_Mock_t testMock0;
   ServiceTest_Mock_t testMock1;
   I_DataModel_t *dataModel;
   ServiceModeTestComponent_t testComponents[2];

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      ServiceTest_Mock_Init(&testMock0, ValidTestNumber0);
      ServiceTest_Mock_Init(&testMock1, ValidTestNumber1);
   }

   void GivenModuleIsInitialized()
   {
      ServiceModeRequestHandler_Init(
         &instance,
         dataModel,
         NumberOfServiceTests);

      ServiceModeRequestHandler_AddTest(
         &instance,
         &testMock0.interface,
         &testComponents[0]);
      ServiceModeRequestHandler_AddTest(
         &instance,
         &testMock1.interface,
         &testComponents[1]);
   }

   void TheNumberOfTestsErdShouldBe(ServiceModeTestNumber_t expected)
   {
      ServiceModeTestNumber_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberMaximum, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheRequestErdShouldBeCleared(void)
   {
      ServiceModeTestRequest_t expected;
      expected.testRequestCommand = ServiceModeTestRequestCommand_Clear;

      ServiceModeTestRequest_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberRequest, &actual);
      CHECK_EQUAL(expected.testRequestCommand, actual.testRequestCommand);
   }

   void WhenTestNumberIsRequestedWithCommand(ServiceModeTestNumber_t testNumber, ServiceModeTestRequestCommand_t command)
   {
      ServiceModeTestRequest_t request;
      request.testNumber = testNumber;
      request.testRequestCommand = command;

      DataModel_Write(dataModel, Erd_ServiceModeTestNumberRequest, &request);
   }

   void TheStatusErdShouldBeTestNotSupported(void)
   {
      ServiceModeTestStatus_t expected;
      expected.testResponse = ServiceModeTestStatusResponse_NotSupported;
      expected.dataFormat = ServiceModeTestStatusDataFormat_Unused;

      ServiceModeTestStatus_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberStatus, &actual);

      CHECK_EQUAL(expected.testResponse, actual.testResponse);
      CHECK_EQUAL(expected.dataFormat, actual.dataFormat);
   }

   void ShouldCallTestsStartFunctionOn(I_ServiceTest_t * interface)
   {
      mock().expectOneCall("Start").onObject(interface);
   }

   void ShouldCallTestsStopFunctionOn(I_ServiceTest_t * interface)
   {
      mock().expectOneCall("Stop").onObject(interface);
   }

   void WhenCallbackIsInvokedWith(ServiceModeTestStatus_t status)
   {
      ServiceTest_Mock_InvokeCallbackForActiveTestWith(&testMock0, status);
   }

   void ServiceTestStatusShouldBeUpdatedWith(ServiceModeTestStatus_t expected)
   {
      ServiceModeTestStatus_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberStatus, &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(ServiceModeTestStatus_t));
   }
};

TEST(ServiceModeRequestHandler, ShouldWriteToMaxNumberOfTestsErdOnInit)
{
   TheNumberOfTestsErdShouldBe(0);

   GivenModuleIsInitialized();
   TheNumberOfTestsErdShouldBe(NumberOfServiceTests);
}

TEST(ServiceModeRequestHandler, ShouldClearServiceTestRequestOnInit)
{
   GivenModuleIsInitialized();
   TheRequestErdShouldBeCleared();
}

TEST(ServiceModeRequestHandler, ShouldClearServiceTestRequestWhenChanged)
{
   GivenModuleIsInitialized();

   WhenTestNumberIsRequestedWithCommand(OutOfBoundTestNumber, ServiceModeTestRequestCommand_Start);
   TheRequestErdShouldBeCleared();
}

TEST(ServiceModeRequestHandler, ShouldSetStatusToTestNotSupportedIfOutsideOfRange)
{
   GivenModuleIsInitialized();

   WhenTestNumberIsRequestedWithCommand(OutOfBoundTestNumber, ServiceModeTestRequestCommand_Start);
   TheStatusErdShouldBeTestNotSupported();
}

TEST(ServiceModeRequestHandler, ShouldSetStatusToTestNotSupportedIfTestIsNotInitialized)
{
   GivenModuleIsInitialized();

   WhenTestNumberIsRequestedWithCommand(NonInitializedTestNumber, ServiceModeTestRequestCommand_Start);
   TheStatusErdShouldBeTestNotSupported();
}

TEST(ServiceModeRequestHandler, ShouldStartAValidTestWhenRequested)
{
   GivenModuleIsInitialized();

   ShouldCallTestsStartFunctionOn(&testMock0.interface);
   WhenTestNumberIsRequestedWithCommand(ValidTestNumber0, ServiceModeTestRequestCommand_Start);

   ShouldCallTestsStartFunctionOn(&testMock1.interface);
   WhenTestNumberIsRequestedWithCommand(ValidTestNumber1, ServiceModeTestRequestCommand_Start);
}

TEST(ServiceModeRequestHandler, ShouldStopAValidTestWhenRequested)
{
   GivenModuleIsInitialized();

   ShouldCallTestsStopFunctionOn(&testMock0.interface);
   WhenTestNumberIsRequestedWithCommand(ValidTestNumber0, ServiceModeTestRequestCommand_Stop);

   ShouldCallTestsStopFunctionOn(&testMock1.interface);
   WhenTestNumberIsRequestedWithCommand(ValidTestNumber1, ServiceModeTestRequestCommand_Stop);
}

TEST(ServiceModeRequestHandler, ShouldSetStatusErdWithDataFromCallback)
{
   GivenModuleIsInitialized();

   ShouldCallTestsStartFunctionOn(&testMock0.interface);
   WhenTestNumberIsRequestedWithCommand(ValidTestNumber0, ServiceModeTestRequestCommand_Start);

   ServiceModeTestStatus_t status;
   status.testResponse = ServiceModeTestStatusResponse_Running;
   status.dataFormat = ServiceModeTestStatusDataFormat_DegreesFx100;
   status.testNumber = ValidTestNumber0;
   status.diagnosticData[0] = 0x12;
   status.diagnosticData[1] = 0x23;
   status.diagnosticData[2] = 0x45;
   status.diagnosticData[3] = 0x67;

   WhenCallbackIsInvokedWith(status);
   ServiceTestStatusShouldBeUpdatedWith(status);
}

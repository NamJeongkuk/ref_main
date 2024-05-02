/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ServiceModeTest_Thermistor.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "ServiceModeTestNumber.h"
#include "SystemErds.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

#define Is ,
#define ChangesTo ,

enum
{
   FF_Index_Thermistor,
   FZ_Index_Thermistor,
   FZEvap_Index_Thermistor,

   SomeThermistorTemperature = 900,
   SomeNegativeThermistorTemperature = -500,
   SomeInvalidThermistorTemperature = 30000,

   TemperatureUpdatePeriodInMsec = 1 * MSEC_PER_SEC
};

static const ServiceModeTestNumber_t testGroupItems[] = {
   ServiceModeTestNumber_FreshFoodCabinetThermistorTest,
   ServiceModeTestNumber_FreezerCabinetThermistorTest,
   ServiceModeTestNumber_FreezerEvaporatorThermistorTest
};

static const ServiceModeTest_TestNumbersMappingTable_t testGroupConfig = {
   .testNumberEntries = testGroupItems,
   .numberOfItems = NUM_ELEMENTS(testGroupItems)
};

static const Erd_t thermistorsDataItems[] = {
   Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   Erd_Freezer_UnfilteredTemperatureInDegFx100,
   Erd_FreezerEvap_UnfilteredTemperatureInDegFx100
};

static const ServiceModeTest_ThermistorMappingConfig_t thermistorsMappingConfig = {
   .unfilteredTemperatureErds = thermistorsDataItems,
   .numberOfItems = NUM_ELEMENTS(thermistorsDataItems)
};

static void TestsResultCallback(void *context, const ServiceTestResultArgs_t *args)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);

   DataModel_Write(dataModel, Erd_ServiceModeTestNumberStatus, &args->status);
}

TEST_GROUP(ServiceModeTest_Thermistor)
{
   ServiceModeTest_Thermistor_t instance;

   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   TimerModule_TestDouble_t *timerModuleTestDouble;

   ServiceTestResources_t resources;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      resources.dataModel = dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      ServiceModeTest_Thermistor_Init(
         &instance,
         &testGroupConfig,
         &thermistorsMappingConfig);
   }

   void WhenTestIsStartedFor(ServiceModeTestNumber_t testNumber, uint8_t itemIndex)
   {
      resources.itemIndex = itemIndex;
      resources.testNumber = testNumber;

      ServiceTest_Start(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void WhenTestIsStoppedFor(ServiceModeTestNumber_t testNumber, uint8_t itemIndex)
   {
      resources.itemIndex = itemIndex;
      resources.testNumber = testNumber;

      ServiceTest_Stop(&instance.interface, dataModel, &resources, TestsResultCallback);
   }

   void GivenTheThermistor(uint8_t thermistorIndex, TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, thermistorsDataItems[thermistorIndex], &temperature);
   }

   void ThenTemperatureOfThermistor(uint8_t thermistorIndex, TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, thermistorsDataItems[thermistorIndex], &temperature);
   }

   void ServiceModeTestNumberStatusShouldBe(ServiceModeTestStatus_t expected)
   {
      ServiceModeTestStatus_t actual;
      DataModel_Read(dataModel, Erd_ServiceModeTestNumberStatus, &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(ServiceModeTestStatus_t));
   }

   void ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_t response,
      ServiceModeTestStatusDataFormat_t dataFormat,
      ServiceModeTestNumber_t testNumber,
      int16_t temperature)
   {
      ServiceModeTestStatus_t testStatus;

      testStatus.testResponse = response;
      testStatus.dataFormat = dataFormat;
      testStatus.testNumber = testNumber;

      testStatus.diagnosticData[0] = temperature >> 8;
      testStatus.diagnosticData[1] = temperature & 0xFF;
      testStatus.diagnosticData[2] = 0;
      testStatus.diagnosticData[3] = 0;

      ServiceModeTestNumberStatusShouldBe(testStatus);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void PeriodicResourceTimerShouldBe(bool expectedState)
   {
      bool timerIsRunning;
      timerIsRunning = TimerModule_IsRunning(TimerModule_TestDouble_GetTimerModule(timerModuleTestDouble), &instance._private.resources->timer);

      CHECK_EQUAL(expectedState, timerIsRunning);
   }
};

TEST(ServiceModeTest_Thermistor, ShouldStoreTestGroupInTheInterface)
{
   GivenTheModuleIsInitialized();

   MEMCMP_EQUAL(&testGroupConfig, instance.interface.testNumbersMappingTable, sizeof(ServiceModeTest_TestNumbersMappingTable_t));
}

TEST(ServiceModeTest_Thermistor, ShouldSetInstantaneousTemperatureBytesToTheResponseWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();
   GivenTheThermistor(FF_Index_Thermistor Is SomeThermistorTemperature);

   WhenTestIsStartedFor(ServiceModeTestNumber_FreshFoodCabinetThermistorTest, FF_Index_Thermistor);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreshFoodCabinetThermistorTest,
      SomeThermistorTemperature);
}

TEST(ServiceModeTest_Thermistor, ShouldSetInstantaneousTemperaturePeriodicallyToTheResponseWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();
   GivenTheThermistor(FZ_Index_Thermistor Is SomeThermistorTemperature);

   WhenTestIsStartedFor(ServiceModeTestNumber_FreezerCabinetThermistorTest, FZ_Index_Thermistor);
   PeriodicResourceTimerShouldBe(ENABLED);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerCabinetThermistorTest,
      SomeThermistorTemperature);

   ThenTemperatureOfThermistor(FZ_Index_Thermistor ChangesTo SomeNegativeThermistorTemperature);
   After(TemperatureUpdatePeriodInMsec - 1);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerCabinetThermistorTest,
      SomeThermistorTemperature);

   After(1);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerCabinetThermistorTest,
      SomeNegativeThermistorTemperature);

   ThenTemperatureOfThermistor(FZ_Index_Thermistor ChangesTo SomeInvalidThermistorTemperature);
   After(TemperatureUpdatePeriodInMsec - 1);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerCabinetThermistorTest,
      SomeNegativeThermistorTemperature);

   After(1);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerCabinetThermistorTest,
      SomeInvalidThermistorTemperature);
}

TEST(ServiceModeTest_Thermistor, ShouldBeAbleToSetNegativeTemperatureValuesToTheResponseWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();
   GivenTheThermistor(FZEvap_Index_Thermistor Is SomeNegativeThermistorTemperature);

   WhenTestIsStartedFor(ServiceModeTestNumber_FreezerEvaporatorThermistorTest, FZEvap_Index_Thermistor);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerEvaporatorThermistorTest,
      SomeNegativeThermistorTemperature);
}

TEST(ServiceModeTest_Thermistor, ShouldBeAbleToSetInvalidTemperatureValuesToTheResponseWhenTestIsStarted)
{
   GivenTheModuleIsInitialized();
   GivenTheThermistor(FZ_Index_Thermistor Is SomeInvalidThermistorTemperature);

   WhenTestIsStartedFor(ServiceModeTestNumber_FreezerCabinetThermistorTest, FZ_Index_Thermistor);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Running,
      ServiceModeTestStatusDataFormat_DegreesFx100,
      ServiceModeTestNumber_FreezerCabinetThermistorTest,
      SomeInvalidThermistorTemperature);
}

TEST(ServiceModeTest_Thermistor, ShouldSetStopToTheResponseWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();
   GivenTheThermistor(FZEvap_Index_Thermistor Is SomeThermistorTemperature);

   WhenTestIsStoppedFor(ServiceModeTestNumber_FreezerEvaporatorThermistorTest, FZEvap_Index_Thermistor);

   ServiceModeThermistorTestDataShouldBe(
      ServiceModeTestStatusResponse_Stopped,
      ServiceModeTestStatusDataFormat_Unused,
      ServiceModeTestNumber_FreezerEvaporatorThermistorTest,
      0);
}

TEST(ServiceModeTest_Thermistor, ShouldStopPeriodicTimerWhenTestIsStopped)
{
   GivenTheModuleIsInitialized();
   GivenTheThermistor(FZEvap_Index_Thermistor Is SomeThermistorTemperature);

   WhenTestIsStartedFor(ServiceModeTestNumber_FreezerEvaporatorThermistorTest, FZEvap_Index_Thermistor);
   PeriodicResourceTimerShouldBe(ENABLED);

   WhenTestIsStoppedFor(ServiceModeTestNumber_FreezerEvaporatorThermistorTest, FZEvap_Index_Thermistor);
   PeriodicResourceTimerShouldBe(DISABLED);
}

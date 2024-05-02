/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModeTest_Thermistor.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "TemperatureDegFx100.h"
#include "utils.h"

enum
{
   TemperatureUpdatePeriodInMsec = 1 * MSEC_PER_SEC
};

static void ReadCurrentThermistorTemperature(void *context)
{
   ServiceModeTest_Thermistor_t *instance = context;

   TemperatureDegFx100_t temperature;

   DataModel_Read(
      instance->_private.resources->dataModel,
      instance->_private.thermistorsMappingConfig->unfilteredTemperatureErds[instance->_private.resources->itemIndex],
      &temperature);

   ServiceTestResultArgs_t testResultArgs;

   testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Running;
   testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_DegreesFx100;
   testResultArgs.status.testNumber = instance->_private.resources->testNumber;

   testResultArgs.status.diagnosticData[0] = temperature >> 8;
   testResultArgs.status.diagnosticData[1] = temperature & 0xFF;
   testResultArgs.status.diagnosticData[2] = 0;
   testResultArgs.status.diagnosticData[3] = 0;

   instance->_private.resources->callback(instance->_private.resources->context, &testResultArgs);
}

static void Start(I_ServiceTest_t *_instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   REINTERPRET(instance, _instance, ServiceModeTest_Thermistor_t *);

   resources->context = context;
   resources->callback = callback;
   instance->_private.resources = resources;

   ReadCurrentThermistorTemperature(instance);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.resources->dataModel, Erd_TimerModule),
      &instance->_private.resources->timer,
      TemperatureUpdatePeriodInMsec,
      ReadCurrentThermistorTemperature,
      instance);
}

static void Stop(I_ServiceTest_t *_instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   REINTERPRET(instance, _instance, ServiceModeTest_Thermistor_t *);

   resources->context = context;
   resources->callback = callback;
   instance->_private.resources = resources;

   ServiceTestResultArgs_t testResultArgs;

   testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Stopped;
   testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testResultArgs.status.testNumber = instance->_private.resources->testNumber;

   testResultArgs.status.diagnosticData[0] = 0;
   testResultArgs.status.diagnosticData[1] = 0;
   testResultArgs.status.diagnosticData[2] = 0;
   testResultArgs.status.diagnosticData[3] = 0;

   callback(context, &testResultArgs);

   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(resources->dataModel, Erd_TimerModule),
      &resources->timer);
}

static const I_ServiceTest_Api_t api = {
   .Start = Start,
   .Stop = Stop
};

void ServiceModeTest_Thermistor_Init(
   ServiceModeTest_Thermistor_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingConfig,
   const ServiceModeTest_ThermistorMappingConfig_t *thermistorsMappingConfig)
{
   instance->interface.api = &api;
   instance->interface.testNumbersMappingTable = testNumbersMappingConfig;

   instance->_private.thermistorsMappingConfig = thermistorsMappingConfig;
}

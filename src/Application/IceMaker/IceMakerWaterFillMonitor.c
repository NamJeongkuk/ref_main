/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceMakerWaterFillMonitor.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "FlowMeterMonitoringRequest.h"
#include "IceMakerWaterFillMonitoringRequest.h"
#include "Signal.h"

static void StopAllActiveTimers(IceMakerWaterFillMonitor_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxFillTimer);
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timedFillTimer);
}

static void FlowMeterMonitoringRequestIsSetTo(
   IceMakerWaterFillMonitor_t *instance,
   FlowMeterMonitoringRequest_t request)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->flowMeterMonitoringRequestErd,
      &request);
}

static void SendStopIceMakerFillSignal(IceMakerWaterFillMonitor_t *instance)
{
   Signal_SendViaErd(
      DataModel_AsDataSource(instance->_private.dataModel),
      instance->_private.config->stopIceMakerFillSignalErd);
}

static void StopMonitoring(void *context)
{
   IceMakerWaterFillMonitor_t *instance = context;

   DataModel_Unsubscribe(
      instance->_private.dataModel,
      instance->_private.config->flowMeterWaterDispensedOzx100Erd,
      &instance->_private.flowMeterWaterDispensedSubscription);
   StopAllActiveTimers(instance);
   SendStopIceMakerFillSignal(instance);

   FlowMeterMonitoringRequestIsSetTo(instance, FlowMeterMonitoringRequest_Stop);
}

static void StartTimers(IceMakerWaterFillMonitor_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timedFillTimer,
      instance->_private.iceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * MSEC_PER_SEC / 10,
      StopMonitoring,
      instance);
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxFillTimer,
      instance->_private.iceMakerFillMonitorData->preciseIceMakerFillMaxTimeInSecondsx10 * MSEC_PER_SEC / 10,
      StopMonitoring,
      instance);
}

static void StartMonitoring(IceMakerWaterFillMonitor_t *instance)
{
   StartTimers(instance);
   FlowMeterMonitoringRequestIsSetTo(instance, FlowMeterMonitoringRequest_Start);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->flowMeterWaterDispensedOzx100Erd,
      &instance->_private.flowMeterWaterDispensedSubscription);
}

static void PauseMonitoring(IceMakerWaterFillMonitor_t *instance)
{
   TimerModule_Pause(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timedFillTimer);
   TimerModule_Pause(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxFillTimer);
   DataModel_Unsubscribe(
      instance->_private.dataModel,
      instance->_private.config->flowMeterWaterDispensedOzx100Erd,
      &instance->_private.flowMeterWaterDispensedSubscription);

   FlowMeterMonitoringRequestIsSetTo(instance, FlowMeterMonitoringRequest_Pause);
}

static void ResumeMonitoring(IceMakerWaterFillMonitor_t *instance)
{
   TimerModule_Resume(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timedFillTimer);
   TimerModule_Resume(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.maxFillTimer);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->flowMeterWaterDispensedOzx100Erd,
      &instance->_private.flowMeterWaterDispensedSubscription);

   FlowMeterMonitoringRequestIsSetTo(instance, FlowMeterMonitoringRequest_Resume);
}

static void OnFillMonitorRequestChange(void *context, const void *_args)
{
   IceMakerWaterFillMonitor_t *instance = context;
   const IceMakerWaterFillMonitoringRequest_t *request = _args;

   switch(*request)
   {
      case IceMakerWaterFillMonitoringRequest_Start:
         StartMonitoring(instance);
         break;

      case IceMakerWaterFillMonitoringRequest_Stop:
         // Do nothing
         break;

      case IceMakerWaterFillMonitoringRequest_Pause:
         PauseMonitoring(instance);
         break;

      case IceMakerWaterFillMonitoringRequest_Resume:
         ResumeMonitoring(instance);
         break;
   }
}

static void OnFlowMeterWaterDispensedChange(void *context, const void *_args)
{
   IceMakerWaterFillMonitor_t *instance = context;
   const uint32_t *amountDispensedInOzx100 = _args;

   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timedFillTimer);

   if(*amountDispensedInOzx100 >= instance->_private.iceMakerFillMonitorData->preciseIceMakerFillOuncesInOzx100)
   {
      StopMonitoring(instance);
   }
}

void IceMakerWaterFillMonitor_Init(
   IceMakerWaterFillMonitor_t *instance,
   I_DataModel_t *dataModel,
   const IceMakerWaterFillMonitorConfig_t *config,
   const IceMakerFillMonitorData_t *iceMakerFillMonitorData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.iceMakerFillMonitorData = iceMakerFillMonitorData;

   EventSubscription_Init(
      &instance->_private.fillMonitoringRequestSubscription,
      instance,
      OnFillMonitorRequestChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->waterFillMonitoringRequestErd,
      &instance->_private.fillMonitoringRequestSubscription);

   EventSubscription_Init(
      &instance->_private.flowMeterWaterDispensedSubscription,
      instance,
      OnFlowMeterWaterDispensedChange);
}

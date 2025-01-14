/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FlowMeterMonitor.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

enum
{
   PollingPeriodInMsec = 100,
   MinimumNumberOfPulsesBeforePublishingOuncesDispensed = 2
};

static uint32_t FlowMeterCounts(FlowMeterMonitor_t *instance)
{
   uint32_t flowMeterCounts;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->flowMeterInputCaptureCountsErd,
      &flowMeterCounts);

   return flowMeterCounts;
}

static void CalculateAndWriteOuncesDispensed(void *context)
{
   FlowMeterMonitor_t *instance = context;

   uint32_t currentFlowMeterCounts = FlowMeterCounts(instance);

   uint32_t countsDifference = 0;
   if(currentFlowMeterCounts < instance->_private.previousFlowMeterCounts) // the counts variable has overflowed
   {
      countsDifference = UINT32_MAX - instance->_private.previousFlowMeterCounts + currentFlowMeterCounts;
   }
   else
   {
      countsDifference = currentFlowMeterCounts - instance->_private.previousFlowMeterCounts;
   }

   instance->_private.flowMeterCountsSinceBeginningMonitoring += countsDifference;

   uint32_t ouncesDispensed = 0;
   if(instance->_private.flowMeterCountsSinceBeginningMonitoring >= MinimumNumberOfPulsesBeforePublishingOuncesDispensed)
   {
      ouncesDispensed = instance->_private.flowMeterData->flowMeterOzPerPulseX100000 * instance->_private.flowMeterCountsSinceBeginningMonitoring / 1000 +
         instance->_private.flowMeterData->flowMeterOffsetOzX100;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->flowMeterWaterDispensedOzX100Erd,
      &ouncesDispensed);
   instance->_private.previousFlowMeterCounts = currentFlowMeterCounts;
}

static void StartPollTimer(FlowMeterMonitor_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.pollTimer,
      PollingPeriodInMsec,
      CalculateAndWriteOuncesDispensed,
      instance);
}

static void StopPollTimer(FlowMeterMonitor_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.pollTimer);
}

static void PausePollTimer(FlowMeterMonitor_t *instance)
{
   TimerModule_Pause(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.pollTimer);
}

static void ResumePollTimer(FlowMeterMonitor_t *instance)
{
   TimerModule_Resume(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.pollTimer);
}

static void MonitoringRequestChanged(void *context, const void *args)
{
   FlowMeterMonitor_t *instance = context;
   const FlowMeterMonitoringRequest_t *request = args;

   switch(*request)
   {
      case FlowMeterMonitoringRequest_Start:
         DataModel_Read(
            instance->_private.dataModel,
            instance->_private.config->flowMeterInputCaptureCountsErd,
            &instance->_private.previousFlowMeterCounts);

         instance->_private.flowMeterCountsSinceBeginningMonitoring = 0;
         StartPollTimer(instance);
         break;

      case FlowMeterMonitoringRequest_Stop:
         StopPollTimer(instance);

         uint32_t zeroOunces = 0;
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->flowMeterWaterDispensedOzX100Erd,
            &zeroOunces);
         break;

      case FlowMeterMonitoringRequest_Pause:
         PausePollTimer(instance);
         break;

      case FlowMeterMonitoringRequest_Resume:
         ResumePollTimer(instance);
         break;
   }
}

void FlowMeterMonitor_Init(
   FlowMeterMonitor_t *instance,
   I_DataModel_t *dataModel,
   const FlowMeterMonitorConfig_t *config,
   const FlowMeterData_t *flowMeterData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.flowMeterData = flowMeterData;

   FlowMeterMonitoringRequest_t monitoringRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->flowMeterMonitoringRequest,
      &monitoringRequest);
   MonitoringRequestChanged(instance, &monitoringRequest);

   EventSubscription_Init(
      &instance->_private.flowMeterMonitoringRequestSubscription,
      instance,
      MonitoringRequestChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->flowMeterMonitoringRequest,
      &instance->_private.flowMeterMonitoringRequestSubscription);
}

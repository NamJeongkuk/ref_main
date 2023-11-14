/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorFaultHandler.h"
#include "utils.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"

static void ClearExcessiveCompressorRunTimeFault(CompressorFaultHandler_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->excessiveCompressorRunTimeFaultErd,
      clear);
}

static void SetExcessiveCompressorRunTimeFault(void *context)
{
   CompressorFaultHandler_t *instance = context;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->excessiveCompressorRunTimeFaultErd,
      set);
}

static void SetCompressorOffWhileCoolingSystemOnForMaxTimeFault(void *context)
{
   CompressorFaultHandler_t *instance = context;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorOffWhileCoolingSystemOnForMaxTimeFaultErd,
      set);
}

static void ClearCompressorOffWhileCoolingSystemOnFault(CompressorFaultHandler_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorOffWhileCoolingSystemOnForMaxTimeFaultErd,
      clear);
}

static void StartCompressorRunningTimer(CompressorFaultHandler_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      instance->_private.compressorTimes->excessiveRunTimeInMinutes * MSEC_PER_MIN,
      SetExcessiveCompressorRunTimeFault,
      instance);
}

static void StopCompressorRunningTimer(CompressorFaultHandler_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer);
}

static void StartCompressorOffCoolingOnMaxAllowedTimeTimer(CompressorFaultHandler_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      instance->_private.compressorTimes->compressorOffCoolingOnMaxAllowedTimeInMinutes * MSEC_PER_MIN,
      SetCompressorOffWhileCoolingSystemOnForMaxTimeFault,
      instance);
}

static void StopCompressorOffCoolingOnMaxAllowedTimeTimer(CompressorFaultHandler_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer);
}

static bool CoolingSystemIsOn(CompressorFaultHandler_t *instance)
{
   bool coolingSystemIsOff;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->coolingOffStatusErd,
      &coolingSystemIsOff);

   return !coolingSystemIsOff;
}

static bool CompressorIsOff(CompressorFaultHandler_t *instance)
{
   bool compressorIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &compressorIsOn);

   return !compressorIsOn;
}

static void OnDataModelChange(void *context, const void *_args)
{
   CompressorFaultHandler_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->compressorIsOnErd)
   {
      const bool *compressorIsOn = args->data;
      if(*compressorIsOn)
      {
         StopCompressorOffCoolingOnMaxAllowedTimeTimer(instance);
         ClearCompressorOffWhileCoolingSystemOnFault(instance);

         if(instance->_private.initialCycleComplete)
         {
            StartCompressorRunningTimer(instance);
         }
      }
      else
      {
         instance->_private.initialCycleComplete = true;

         StopCompressorRunningTimer(instance);
         ClearExcessiveCompressorRunTimeFault(instance);

         if(CoolingSystemIsOn(instance))
         {
            StartCompressorOffCoolingOnMaxAllowedTimeTimer(instance);
         }
      }
   }
   else if(args->erd == instance->_private.config->coolingOffStatusErd)
   {
      const bool *coolingSystemIsOff = args->data;
      if(*coolingSystemIsOff)
      {
         StopCompressorOffCoolingOnMaxAllowedTimeTimer(instance);
      }
      else if(CompressorIsOff(instance))
      {
         StartCompressorOffCoolingOnMaxAllowedTimeTimer(instance);
      }
   }
}

void CompressorFaultHandler_Init(
   CompressorFaultHandler_t *instance,
   I_DataModel_t *dataModel,
   const CompressorFaultHandlerConfig_t *config,
   const CompressorTimes_t *compressorTimes)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.initialCycleComplete = false;
   instance->_private.compressorTimes = compressorTimes;

   if(CompressorIsOff(instance) && CoolingSystemIsOn(instance))
   {
      StartCompressorOffCoolingOnMaxAllowedTimeTimer(instance);
   }

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}

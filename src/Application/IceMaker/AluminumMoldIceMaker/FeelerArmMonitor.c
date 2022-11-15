/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeelerArmMonitor.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "FeelerArmPosition.h"
#include "PersonalityParametricData.h"

static void FeelerArmDelayTimerExpired(void *context)
{
   FeelerArmMonitor_t *instance = context;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->feelerArmIsReadyToEnterHarvestErd,
      set);
}

static bool FeelerArmPositionIsBucketEmpty(FeelerArmMonitor_t *instance)
{
   FeelerArmPosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->feelerArmPositionErd,
      &position);

   return position == FeelerArmPosition_BucketEmpty;
}

static bool FeelerArmMonitoringIsRequested(FeelerArmMonitor_t *instance)
{
   bool feelerArmMonitoringRequested;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->feelerArmMonitoringRequestErd,
      &feelerArmMonitoringRequested);

   return feelerArmMonitoringRequested;
}

static void FeelerArmMonitoringRequestOrFeelerArmPositionUpdated(void *context, const void *args)
{
   FeelerArmMonitor_t *instance = context;
   IGNORE(args);

   if(FeelerArmMonitoringIsRequested(instance) && FeelerArmPositionIsBucketEmpty(instance))
   {
      TimerModule_StartOneShot(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            instance->_private.config->timerModuleErd),
         &instance->_private.feelerArmDelayTimer,
         instance->_private.aluminumMoldIceMakerData->minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN,
         FeelerArmDelayTimerExpired,
         instance);
   }
   else
   {
      TimerModule_Stop(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            instance->_private.config->timerModuleErd),
         &instance->_private.feelerArmDelayTimer);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->feelerArmIsReadyToEnterHarvestErd,
         clear);
   }
}

void FeelerArmMonitor_Init(
   FeelerArmMonitor_t *instance,
   I_DataModel_t *dataModel,
   const FeelerArmMonitorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.aluminumMoldIceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;

   EventSubscription_Init(
      &instance->_private.feelerArmMonitoringRequestSubscription,
      instance,
      FeelerArmMonitoringRequestOrFeelerArmPositionUpdated);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->feelerArmMonitoringRequestErd,
      &instance->_private.feelerArmMonitoringRequestSubscription);

   EventSubscription_Init(
      &instance->_private.feelerArmPositionSubscription,
      instance,
      FeelerArmMonitoringRequestOrFeelerArmPositionUpdated);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->feelerArmPositionErd,
      &instance->_private.feelerArmPositionSubscription);
}

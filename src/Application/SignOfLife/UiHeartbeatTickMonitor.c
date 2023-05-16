/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UiHeartbeatTickMonitor.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"

static void SetThatThereIsCommunicationWithTheUi(void *context)
{
   UiHeartbeatTickMonitor_t *instance = context;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->activeCommunicationWithUiErd,
      set);
}

static void ClearCommunicationWithTheUi(void *context)
{
   UiHeartbeatTickMonitor_t *instance = context;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->activeCommunicationWithUiErd,
      clear);
}

static void UiSignOfLifeChanged(void *context, const void *args)
{
   UiHeartbeatTickMonitor_t *instance = context;
   IGNORE(args);
   SetThatThereIsCommunicationWithTheUi(instance);

   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.communicationTimeoutTimer,
      instance->_private.config->maxUiUpdateIntervalInMsec,
      ClearCommunicationWithTheUi,
      instance);
}

void UiHeartbeatTickMonitor_Init(
   UiHeartbeatTickMonitor_t *instance,
   const UiHeartbeatTickMonitorConfiguration_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ClearCommunicationWithTheUi(instance);

   EventSubscription_Init(
      &instance->_private.uiHeartbeatChangedSubscription,
      instance,
      UiSignOfLifeChanged);
   DataModel_Subscribe(
      dataModel,
      config->heartbeatTickFromUiErd,
      &instance->_private.uiHeartbeatChangedSubscription);

   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         Erd_TimerModule),
      &instance->_private.communicationTimeoutTimer,
      instance->_private.config->maxUiUpdateIntervalInMsec,
      ClearCommunicationWithTheUi,
      instance);
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SignOfLifePlugin.h"
#include "SystemErds.h"

enum
{
   HeartbeatTickPeriodInMsec = 1500,
   MaxUpdateIntervalInMsec = 1500
};

static const UiHeartbeatTickMonitorConfiguration_t uiHeartbeatTickMonitorConfiguration = {
   .heartbeatTickFromUiErd = Erd_AndroidUiSignOfLife,
   .activeCommunicationWithUiErd = Erd_MainboardIsCommunicatingToAndroidUi,
   .maxUiUpdateIntervalInMsec = MaxUpdateIntervalInMsec
};

void SignOfLifePlugin_Init(SignOfLifePlugin_t *instance, I_DataModel_t *dataModel)
{
   UiHeartbeatTickMonitor_Init(
      &instance->_private.uiHeartbeatTickMonitor,
      &uiHeartbeatTickMonitorConfiguration,
      dataModel);
}

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
   MaxUiUpdateIntervalInMinutes = 2,
   MaxTimeBeforeUiResetAttemptInMinutes = 10,
   AndroidUiCommunicationFaultTimeInMinutes = 15
};

static const AndroidUiSignOfLifeMonitorConfiguration_t androidUiSignOfLifeMonitorConfiguration = {
   .heartbeatTickFromUiErd = Erd_PrimaryUiSignOfLife,
   .uiSignOfLifeFsmStateErd = Erd_AndroidUiSignOfLifeFsmState,
   .mainboardIsConnectedToUiNvErd = Erd_MainboardIsConnectedToTheAndroidUi,
   .numberOfTimesMainboardHasLostCommunicationWithUi = Erd_NumberOfTimesMainboardHasLostCommunicationWithAndroidUi,
   .numberOfTimesMainboardHasAttemptedToResetUi = Erd_NumberOfTimesMainboardHasAttemptedToResetAndroidUi,
   .uiResetSignalErd = Erd_AndroidUiResetSignal,
   .maxUiUpdateIntervalInMinutes = MaxUiUpdateIntervalInMinutes,
   .resetTimeInMinutes = MaxTimeBeforeUiResetAttemptInMinutes,
   .faultTimeInMinutes = AndroidUiCommunicationFaultTimeInMinutes
};

void SignOfLifePlugin_Init(SignOfLifePlugin_t *instance, I_DataModel_t *dataModel)
{
   AndroidUiSignOfLifeMonitor_Init(
      &instance->_private.androidUiSignOfLifeMonitor,
      &androidUiSignOfLifeMonitorConfiguration,
      dataModel);
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RfidCommunicationControllerPlugin.h"
#include "SystemErds.h"
#include "RfidFilterUpdateRateData.h"

static const RfidCommunicationControllerConfig_t rfidCommunicationControllerConfig = {
   .rfidCommunicationControllerStateErd = Erd_RfidCommunicationControllerState,
   .rfidFilterReadWriteResultErd = Erd_RfidFilterReadWriteResult_RfidBoard,
   .allFreshFoodDoorsAreClosedErd = Erd_AllFreshFoodDoorsAreClosed,
   .rfidFilterUidRfidBoardErd = Erd_RfidFilterUid_RfidBoard,
   .rfidFilterUidErd = Erd_RfidFilterUid,
   .rfidFilterStatusRfidBoardErd = Erd_RfidFilterStatus_RfidBoard,
   .rfidFilterIdentifierErd = Erd_RfidFilterIdentifier_RfidBoard,
   .demoModeEnableErd = Erd_EnableDemoModeStatus,
   .waterFilterTypeErd = Erd_WaterFilterType,
   .rfidFaultRequestErd = Erd_RfidFaultRequest,
   .rfidBoardTagAuthenticationFailedFault = Erd_RfidBoardTagAuthenticationFailedFault,
   .rfidBoardHardwareFailureFaultErd = Erd_RfidBoardHardwareFailureFault,
   .rfidBoardLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
   .rfidBoardBlockedTagFaultErd = Erd_RfidBoardBlockedTagFault,
   .rfidFilterDataRequestErd = Erd_RfidFilterDataRequest,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
};

void RfidCommunicationControllerPlugin_Init(
   RfidCommunicationControllerPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   RfidCommunicationController_Init(
      &instance->_private.rfidCommunicationController,
      dataModel,
      &rfidCommunicationControllerConfig,
      PersonalityParametricData_Get(dataModel)->filterData->rfidFilterData->rfidFilterUpdateRateData);
}

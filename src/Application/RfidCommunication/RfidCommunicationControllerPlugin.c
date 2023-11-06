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
   .rfidFilterBadReadCountErd = Erd_RfidFilterBadReadCount,
   .rfidFilterBadWriteCountErd = Erd_RfidFilterBadWriteCount,
   .rfidFilterHardwareFailureCountErd = Erd_RfidFilterHardwareFailureCount,
   .rfidFilterLeakDetectedCountErd = Erd_RfidFilterLeakDetectCount,
   .rfidFilterBlockedCountErd = Erd_RfidFilterBlockedCount,
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

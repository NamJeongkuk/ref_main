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
   .rfidFilterUnitSerialNumberRfidBoardErd = Erd_RfidFilterUnitSerialNumber_RfidBoard,
   .rfidFilterUnitSerialNumberErd = Erd_RfidFilterUnitSerialNumber,
   .rfidFilterCalendarUsageInSecondsRfidBoardErd = Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,
   .rfidFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .eepromWaterFilterCalendarUsageInSecondsErd = Erd_Eeprom_WaterFilterCalendarUsageInSeconds,
   .rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd = Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard,
   .totalWaterVolumeUsageInOuncesX100Erd = Erd_TotalWaterVolumeUsageInOuncesX100,
   .eepromTotalWaterVolumeUsageInOuncesX100Erd = Erd_Eeprom_TotalWaterVolumeUsageInOuncesX100,
   .rfidFilterStatusRfidBoardErd = Erd_RfidFilterStatus_RfidBoard,
   .rfidFilterIdentifierErd = Erd_RfidFilterIdentifier_RfidBoard,
   .demoModeEnableErd = Erd_EnableDemoModeStatus,
   .waterFilterTypeErd = Erd_WaterFilterType,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .rfidFilterBadReadCountErd = Erd_RfidFilterBadReadCount,
   .rfidFilterBadWriteCountErd = Erd_RfidFilterBadWriteCount,
   .rfidFilterHardwareFailureCountErd = Erd_RfidFilterHardwareFailureCount,
   .rfidFilterLeakDetectedCountErd = Erd_RfidFilterLeakDetectCount,
   .rfidFilterBlockedCountErd = Erd_RfidFilterBlockedCount,
   .rfidFilterDataRequestErd = Erd_RfidFilterDataRequest,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
   .rfidFilterNumberOfUnitsFilterHasBeenOnErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,
   .rfidFilterPreviousUnitSerialNumberErd = Erd_RfidFilterPreviousUnitSerialNumber
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

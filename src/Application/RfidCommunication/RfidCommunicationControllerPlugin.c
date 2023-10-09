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
   .rfidFilterUidMainboardErd = Erd_RfidFilterUid_Mainboard,
   .rfidFilterUnitSerialNumberRfidBoardErd = Erd_RfidFilterUnitSerialNumber_RfidBoard,
   .rfidFilterUnitSerialNumberMainboardErd = Erd_RfidFilterUnitSerialNumber_Mainboard,
   .rfidFilterCalendarusageInSecondsRfidBoardErd = Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,
   .rfidFilterCalendarusageInSecondsMainboardErd = Erd_RfidFilterCalendarUsageInSeconds_Mainboard,
   .rfidFilterWaterVolumeUsageInOuncesRfidBoardErd = Erd_RfidFilterWaterVolumeUsageInOunces_RfidBoard,
   .rfidFilterWaterVolumeUsageInOuncesMainboardErd = Erd_RfidFilterWaterVolumeUsageInOunces_Mainboard,
   .rfidFilterStatusRfidBoardErd = Erd_RfidFilterStatus_RfidBoard,
   .rfidFilterIdentifierErd = Erd_RfidFilterIdentifier_RfidBoard,
   .demoModeEnableErd = Erd_DemoModeEnable,
   .waterFilterTypeErd = Erd_WaterFilterType,
   .bypassPlugErd = Erd_BypassPlugInstalled,
   .filterErrorErd = Erd_FilterError,
   .rfidFilterBadReadCountErd = Erd_RfidFilterBadReadCount,
   .rfidFilterBadWriteCountErd = Erd_RfidFilterBadWriteCount,
   .rfidFilterHardwareFailureCountErd = Erd_RfidFilterHardwareFailureCount,
   .rfidFilterLeakDetectedCountErd = Erd_RfidFilterLeakDetectCount,
   .rfidFilterBlockedCountErd = Erd_RfidFilterBlockedCount,
   .rfidFilterDataRequestErd = Erd_RfidFilterDataRequest,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
   .rfidFilterNumberOfUnitsFilterHasBeenOnMainboardErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_Mainboard,
   .rfidFilterPreviousUnitSerialNumberMainboardErd = Erd_RfidFilterPreviousUnitSerialNumber_Mainboard
};

static void CopyUnitSerialNumberToRfidFilter(I_DataModel_t *dataModel)
{
   SerialNumber_t unitSerialNumber;
   DataModel_Read(
      dataModel,
      Erd_SerialNumber,
      &unitSerialNumber);

   UnitSerialNumber_t rfidFilterUnitSerialNumber;
   memcpy(&rfidFilterUnitSerialNumber, &unitSerialNumber, sizeof(UnitSerialNumber_t));

   DataModel_Write(
      dataModel,
      Erd_RfidFilterUnitSerialNumber_Mainboard,
      &rfidFilterUnitSerialNumber);
}

void RfidCommunicationControllerPlugin_Init(
   RfidCommunicationControllerPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   CopyUnitSerialNumberToRfidFilter(dataModel);

   RfidCommunicationController_Init(
      &instance->_private.rfidCommunicationController,
      dataModel,
      &rfidCommunicationControllerConfig,
      PersonalityParametricData_Get(dataModel)->filterData->rfidFilterData->rfidFilterUpdateRateData);
}

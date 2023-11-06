/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterPlugin.h"
#include "SystemErds.h"

static const WaterFilterCalendarUsageUpdaterConfig_t waterFilterCalendarUsageUpdaterConfig = {
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .lifetimeTotalWaterUsageInOuncesX100Erd = Erd_LifetimeTotalWaterVolumeUsageInOuncesX100,
   .serviceDiagnosticsRunTimeInMinutesErd = Erd_ServiceDiagnosticsRunTimeInMinutes
};

static const WaterVolumeUsageCalculatorConfig_t aluminumMoldIceMakerWaterVolumeUsageCalculatorConfig = {
   .waterValveRelayErd = Erd_AluminumMoldIceMakerWaterValveRelay,
   .waterVolumeUsageInOuncesX100Erd = Erd_LastAluminumMoldIceMakerWaterVolumeUsageInOuncesX100
};

static const WaterVolumeUsageCalculatorConfig_t twistTrayIceMakerWaterVolumeUsageCalculatorConfig = {
   .waterValveRelayErd = Erd_TwistTrayIceMakerWaterValveRelay,
   .waterVolumeUsageInOuncesX100Erd = Erd_LastTwistTrayIceMakerWaterVolumeUsageInOuncesX100
};

static const WaterVolumeUsageCalculatorConfig_t dispensedWaterVolumeUsageCalculatorConfig = {
   .waterValveRelayErd = Erd_DispenserValveRelay,
   .waterVolumeUsageInOuncesX100Erd = Erd_LastDispensedWaterVolumeUsageInOuncesX100
};

static const DispensingDisablerConfig_t dispensingDisablerConfig = {
   .rfidBoardLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
   .dispensingInhibitedReasonBitmapErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonBitmapErd = Erd_IceMakerFillInhibitedReason
};

static const NewFilterInstalledHandlerReadErds_t newFilterInstalledHandlerReadErdsConfig = {
   .rfidFilterUidRfidBoardErd = Erd_RfidFilterUid_RfidBoard,
   .rfidFilterWaterVolumeUsageInOuncesX100RfidBoardErd = Erd_RfidFilterWaterVolumeUsageInOuncesX100_RfidBoard,
   .rfidFilterCalendarUsageInSecondsRfidBoardErd = Erd_RfidFilterCalendarUsageInSeconds_RfidBoard,
   .rfidFilterLastTwelveMonthsOfWaterUsageInGallonsRfidBoardErd = Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons_RfidBoard,
   .rfidFilterNumberOfUnitsRfidFilterHasBeenOnRfidBoardErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn_RfidBoard,
   .rfidFilterUnitSerialNumberRfidBoardErd = Erd_RfidFilterUnitSerialNumber_RfidBoard
};

static const NewFilterInstalledHandlerWriteErds_t newFilterInstalledHandlerWriteErdsConfig = {
   .rfidFilterUidErd = Erd_RfidFilterUid,
   .totalWaterVolumeUsageInOuncesX100Erd = Erd_TotalWaterVolumeUsageInOuncesX100,
   .eepromTotalWaterVolumeUsageInOuncesX100Erd = Erd_Eeprom_TotalWaterVolumeUsageInOuncesX100,
   .rfidFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .eepromWaterFilterCalendarUsageInSecondsErd = Erd_Eeprom_WaterFilterCalendarUsageInSeconds,
   .rfidFilterLastTwelveMonthsOfWaterUsageInGallonsErd = Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons,
   .rfidFilterNumberOfUnitsFilterHasBeenOnErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,
   .rfidFilterPreviousUnitSerialNumberErd = Erd_RfidFilterPreviousUnitSerialNumber
};

static const NewFilterInstalledHandlerConfig_t newFilterInstalledHandlerConfig = {
   .readErds = &newFilterInstalledHandlerReadErdsConfig,
   .writeErds = &newFilterInstalledHandlerWriteErdsConfig,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
   .rfidBoardInSystemErd = Erd_RfidBoardInSystem,
   .rfidFilterIdentifierRfidBoardErd = Erd_RfidFilterIdentifier_RfidBoard,
   .rfidFilterLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
   .rfidFilterBlockedTagFaultErd = Erd_RfidBoardBlockedTagFault,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .rfidFilterUnitSerialNumberErd = Erd_RfidFilterUnitSerialNumber
};

void WaterFilterPlugin_Init(
   WaterFilterPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const WaterValveFlowRate_t *waterValveFlowRateData =
      PersonalityParametricData_Get(dataModel)->waterValveFlowRate;

   WaterFilterCalendarUsageUpdater_Init(
      &instance->_private.waterFilterCalendarUsageUpdater,
      dataModel,
      &waterFilterCalendarUsageUpdaterConfig,
      PersonalityParametricData_Get(dataModel)->filterData->commonFilterData);

   RfidCommunicationPlugin_Init(
      &instance->_private.rfidCommunicationPlugin,
      dataModel);

   WaterVolumeUsageCalculator_Init(
      &instance->_private.aluminumMoldIceMakerWaterVolumeUsageCalculator,
      dataModel,
      &aluminumMoldIceMakerWaterVolumeUsageCalculatorConfig,
      waterValveFlowRateData->aluminumMoldIcemakerValveFlowRateOuncesPerSecX100);

   WaterVolumeUsageCalculator_Init(
      &instance->_private.twistTrayIceMakerWaterVolumeUsageCalculator,
      dataModel,
      &twistTrayIceMakerWaterVolumeUsageCalculatorConfig,
      waterValveFlowRateData->twistTrayIcemakerValveFlowRateOuncesPerSecX100);

   WaterVolumeUsageCalculator_Init(
      &instance->_private.dispensedWaterVolumeUsageCalculator,
      dataModel,
      &dispensedWaterVolumeUsageCalculatorConfig,
      waterValveFlowRateData->dispenserValveFlowRateOuncesPerSecX100);

   DispensingDisabler_Init(
      &instance->_private.dispensingDisabler,
      dataModel,
      &dispensingDisablerConfig);

   NewFilterInstalledHandler_Init(
      &instance->_private.newFilterInstalledHandler,
      dataModel,
      &newFilterInstalledHandlerConfig);
}

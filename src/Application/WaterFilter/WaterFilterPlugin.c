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
   .lifetimeTotalWaterUsageInOuncesX100Erd = Erd_UnitLifetimeWaterVolumeUsageInOuncesX100,
   .serviceDiagnosticsRunTimeInMinutesErd = Erd_ServiceDiagnosticsRunTimeInMinutes
};

static const WaterVolumeUsageCalculatorConfig_t aluminumMoldIceMakerWaterVolumeUsageCalculatorConfig = {
   .waterValveRelayErd = Erd_AluminumMoldIceMakerWaterValveRelay,
   .waterValveOnTimeInSecondsErd = Erd_LastAluminumMoldIceMakerWaterValveOnTimeInSeconds,
   .waterVolumeUsageInOuncesX100Erd = Erd_LastAluminumMoldIceMakerWaterVolumeUsageInOuncesX100
};

static const WaterVolumeUsageCalculatorConfig_t twistTrayIceMakerWaterVolumeUsageCalculatorConfig = {
   .waterValveRelayErd = Erd_TwistTrayIceMakerWaterValveRelay,
   .waterValveOnTimeInSecondsErd = Erd_LastTwistTrayIceMakerWaterValveOnTimeInSeconds,
   .waterVolumeUsageInOuncesX100Erd = Erd_LastTwistTrayIceMakerWaterVolumeUsageInOuncesX100
};

static const WaterVolumeUsageCalculatorConfig_t dispensedWaterVolumeUsageCalculatorConfig = {
   .waterValveRelayErd = Erd_DispenserValveRelay,
   .waterValveOnTimeInSecondsErd = Erd_LastDispensedWaterValveOnTimeInSeconds,
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
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .eepromwaterFilterVolumeUsageInOuncesX100Erd = Erd_Eeprom_WaterFilterVolumeUsageInOuncesX100,
   .rfidFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .eepromWaterFilterCalendarUsageInSecondsErd = Erd_Eeprom_WaterFilterCalendarUsageInSeconds,
   .rfidFilterLastTwelveMonthsOfWaterUsageInGallonsErd = Erd_RfidFilterLastTwelveMonthsOfWaterUsageInGallons,
   .rfidFilterNumberOfUnitsFilterHasBeenOnErd = Erd_RfidFilterNumberOfUnitsRfidFilterHasBeenOn,
   .rfidFilterPreviousUnitSerialNumberErd = Erd_RfidFilterPreviousUnitSerialNumber,
};

static const NewFilterInstalledHandlerConfig_t newFilterInstalledHandlerConfig = {
   .readErds = &newFilterInstalledHandlerReadErdsConfig,
   .writeErds = &newFilterInstalledHandlerWriteErdsConfig,
   .newFilterInstalledSignalErd = Erd_NewFilterInstalledSignal,
   .rfidBoardInSystemErd = Erd_RfidBoardInSystem,
   .rfidFilterIdentifierRfidBoardErd = Erd_RfidFilterIdentifier_RfidBoard,
   .rfidFaultRequestErd = Erd_RfidFaultRequest,
   .rfidFilterLeakDetectedFaultErd = Erd_RfidBoardLeakDetectedFault,
   .rfidFilterBlockedTagFaultErd = Erd_RfidBoardBlockedTagFault,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .rfidFilterUnitSerialNumberErd = Erd_RfidFilterUnitSerialNumber
};

static const Erd_t waterVolumeUsageInOuncesX100Erds[] = {
   Erd_LastAluminumMoldIceMakerWaterVolumeUsageInOuncesX100,
   Erd_LastTwistTrayIceMakerWaterVolumeUsageInOuncesX100,
   Erd_LastDispensedWaterVolumeUsageInOuncesX100
};

static const ErdList_t waterVolumeUsageInOuncesX100ErdList = {
   .erds = waterVolumeUsageInOuncesX100Erds,
   .numberOfErds = NUM_ELEMENTS(waterVolumeUsageInOuncesX100Erds)
};

static const ErdAccumulatorServiceConfig_t waterFilterValveAccumulatorConfig = {
   .inputErdList = waterVolumeUsageInOuncesX100ErdList,
   .cumulativeValueErd = Erd_WaterFilterVolumeUsageInOuncesX100
};

static const ErdAccumulatorServiceConfig_t unitLifetimeWaterValveAccumulatorConfig = {
   .inputErdList = waterVolumeUsageInOuncesX100ErdList,
   .cumulativeValueErd = Erd_UnitLifetimeWaterVolumeUsageInOuncesX100
};

static const Erd_t unitLifetimeDispensedWaterVolumeUsageInOuncesX100Erds[] = {
   Erd_LastDispensedWaterVolumeUsageInOuncesX100
};

static const ErdList_t unitLifetimeDispensedWaterVolumeUsageInOuncesX100ErdList = {
   .erds = unitLifetimeDispensedWaterVolumeUsageInOuncesX100Erds,
   .numberOfErds = NUM_ELEMENTS(unitLifetimeDispensedWaterVolumeUsageInOuncesX100Erds)
};

static const ErdAccumulatorServiceConfig_t unitLifetimeDispensedWaterValveAccumulatorConfig = {
   .inputErdList = unitLifetimeDispensedWaterVolumeUsageInOuncesX100ErdList,
   .cumulativeValueErd = Erd_UnitLifetimeDispensedWaterInOuncesX100
};

static const WaterFilterStateResolverConfig_t waterFilterStateResolverConfig = {
   .enableDemoModeStatusErd = Erd_EnableDemoModeStatus,
   .leakDetectedErd = Erd_LeakDetected,
   .filterErrorErd = Erd_FilterError,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus,
   .resolvedFilterStateErd = Erd_WaterFilterState
};

static const WaterFilterRemainingUsageUpdaterConfig_t waterFilterRemainingUsageUpdaterConfig = {
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .waterFilterRemainingUsageErd = Erd_WaterFilterRemainingUsage
};

static const WaterFilterUsageSinceExpirationUpdaterConfig_t waterFilterUsageSinceExpirationUpdaterConfig = {
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus,
   .waterFilterUsageSinceExpirationErd = Erd_WaterFilterUsageSinceExpiration,
   .calendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100
};

static const IceMakerFillBlockerConfig_t iceMakerFillBlockerConfig = {
   .unitLifetimeDispensedWaterInOuncesX100Erd = Erd_UnitLifetimeDispensedWaterInOuncesX100,
   .waterFilterVolumeUsageInOuncesX100Erd = Erd_WaterFilterVolumeUsageInOuncesX100,
   .waterFilterCalendarUsageInSecondsErd = Erd_WaterFilterCalendarUsageInSeconds,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason
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

   ErdAccumulatorService_Init(
      &instance->_private.unitLifetimeWaterValveAccumulator,
      dataModel,
      &unitLifetimeWaterValveAccumulatorConfig,
      sizeof(VolumeInOuncesX100_t),
      IS_SIGNED(VolumeInOuncesX100_t));

   ErdAccumulatorService_Init(
      &instance->_private.waterFilterValveAccumulator,
      dataModel,
      &waterFilterValveAccumulatorConfig,
      sizeof(VolumeInOuncesX100_t),
      IS_SIGNED(VolumeInOuncesX100_t));

   ErdAccumulatorService_Init(
      &instance->_private.unitLifetimeDispensedWaterValveAccumulator,
      dataModel,
      &unitLifetimeDispensedWaterValveAccumulatorConfig,
      sizeof(VolumeInOuncesX100_t),
      IS_SIGNED(VolumeInOuncesX100_t));

   WaterFilterStateResolver_Init(
      &instance->_private.waterFilterStateResolver,
      dataModel,
      &waterFilterStateResolverConfig);

   WaterFilterRemainingUsageUpdater_Init(
      &instance->_private.waterFilterRemainingUsageUpdater,
      dataModel,
      &waterFilterRemainingUsageUpdaterConfig,
      PersonalityParametricData_Get(dataModel)->filterData->commonFilterData);

   WaterFilterUsageSinceExpirationUpdater_Init(
      &instance->_private.waterFilterUsageSinceExpirationUpdater,
      dataModel,
      &waterFilterUsageSinceExpirationUpdaterConfig,
      PersonalityParametricData_Get(dataModel)->filterData->commonFilterData);

   IceMakerFillBlocker_Init(
      &instance->_private.iceMakerFillBlocker,
      dataModel,
      &iceMakerFillBlockerConfig,
      PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerFillBlockerData);
}

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
}

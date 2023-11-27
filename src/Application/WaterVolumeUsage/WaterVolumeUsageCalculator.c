/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterVolumeUsageCalculator.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"

static void ClearWaterVolumeUsageInOuncesX100(WaterVolumeUsageCalculator_t *instance)
{
   VolumeInOuncesX100_t waterVolumeUsageInOuncesX100 = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterVolumeUsageInOuncesX100Erd,
      &waterVolumeUsageInOuncesX100);
}

static void StartWaterValveRelayOnStopwatch(WaterVolumeUsageCalculator_t *instance)
{
   Stopwatch_Start(&instance->_private.waterValveOnStopwatch);
}

static void StopWaterValveRelayOnStopwatch(WaterVolumeUsageCalculator_t *instance)
{
   Stopwatch_Stop(&instance->_private.waterValveOnStopwatch);
}

static TimerTicks_t WaterValveRelayOnInMilliseconds(WaterVolumeUsageCalculator_t *instance)
{
   return (Stopwatch_ElapsedTime(&instance->_private.waterValveOnStopwatch));
}

static void ResetWaterValveRelayOnStopwatch(WaterVolumeUsageCalculator_t *instance)
{
   Stopwatch_Reset(&instance->_private.waterValveOnStopwatch);
}

static void CalculateWaterVolumeUsageInOuncesX100(WaterVolumeUsageCalculator_t *instance)
{
   VolumeInOuncesX100_t waterVolumeUsageInOuncesX100;
   waterVolumeUsageInOuncesX100 =
      (WaterValveRelayOnInMilliseconds(instance) * (uint64_t)instance->_private.waterValveFlowRateInOuncesPerSecondX100) / MSEC_PER_SEC;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterVolumeUsageInOuncesX100Erd,
      &waterVolumeUsageInOuncesX100);
}

static void WriteWaterValveOnTime(WaterVolumeUsageCalculator_t *instance)
{
   uint32_t waterValveOnTimeInSeconds = (WaterValveRelayOnInMilliseconds(instance) / MSEC_PER_SEC);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterValveOnTimeInSecondsErd,
      &waterValveOnTimeInSeconds);
}

static void ClearWaterValveOnTime(WaterVolumeUsageCalculator_t *instance)
{
   uint32_t waterValveOnTimeInSeconds = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterValveOnTimeInSecondsErd,
      &waterValveOnTimeInSeconds);
}

static void WaterValveRelayChanged(void *context, const void *args)
{
   WaterVolumeUsageCalculator_t *instance = context;
   const bool *waterValveRelayIsOn = args;

   if(*waterValveRelayIsOn)
   {
      StartWaterValveRelayOnStopwatch(instance);
      ClearWaterValveOnTime(instance);
      ClearWaterVolumeUsageInOuncesX100(instance);
   }
   else
   {
      StopWaterValveRelayOnStopwatch(instance);
      WriteWaterValveOnTime(instance);
      CalculateWaterVolumeUsageInOuncesX100(instance);
      ResetWaterValveRelayOnStopwatch(instance);
   }
}

static bool WaterValveRelayIsOn(WaterVolumeUsageCalculator_t *instance)
{
   bool waterValveRelayIsOn;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterValveRelayErd,
      &waterValveRelayIsOn);

   return waterValveRelayIsOn;
}

void WaterVolumeUsageCalculator_Init(
   WaterVolumeUsageCalculator_t *instance,
   I_DataModel_t *dataModel,
   const WaterVolumeUsageCalculatorConfig_t *config,
   const uint8_t waterValveFlowRateInOuncesPerSecondX100)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.waterValveFlowRateInOuncesPerSecondX100 = waterValveFlowRateInOuncesPerSecondX100;

   Stopwatch_Init(
      &instance->_private.waterValveOnStopwatch,
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         Erd_TimerModule));

   if(WaterValveRelayIsOn(instance))
   {
      StartWaterValveRelayOnStopwatch(instance);
   }

   EventSubscription_Init(
      &instance->_private.waterValveRelaySubscription,
      instance,
      WaterValveRelayChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->waterValveRelayErd,
      &instance->_private.waterValveRelaySubscription);
}

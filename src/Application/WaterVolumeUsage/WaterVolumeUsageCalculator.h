/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERVOLUMEUSAGECALCULATOR_H
#define WATERVOLUMEUSAGECALCULATOR_H

#include "I_DataModel.h"
#include "WaterValveFlowRate.h"
#include "Stopwatch.h"

typedef struct
{
   Erd_t waterValveRelayErd; // bool
   Erd_t waterValveOnTimeInSecondsErd; // uint32_t
   Erd_t waterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
} WaterVolumeUsageCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Stopwatch_t waterValveOnStopwatch;
      TimerTicks_t waterValveRelayTicks;
      const WaterVolumeUsageCalculatorConfig_t *config;
      EventSubscription_t waterValveRelaySubscription;
      uint8_t waterValveFlowRateInOuncesPerSecondX100;
   } _private;
} WaterVolumeUsageCalculator_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param waterValveFlowRateInOuncesPerSecondX100 valve flow rate from parametric data
 */
void WaterVolumeUsageCalculator_Init(
   WaterVolumeUsageCalculator_t *instance,
   I_DataModel_t *dataModel,
   const WaterVolumeUsageCalculatorConfig_t *config,
   const uint8_t waterValveFlowRateInOuncesPerSecondX100);

#endif

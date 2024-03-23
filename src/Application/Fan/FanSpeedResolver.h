/*!
 * @file
 * @brief calculates the requested fan speed.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANSPEEDRESOLVER_H
#define FANSPEEDRESOLVER_H

#include "I_DataModel.h"
#include "FanData.h"
#include "FanControl.h"

typedef struct
{
   Erd_t resolvedFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t freezerSetpointErd; // FreezerSetpoint_t
   Erd_t calculatedRequestFanControlErd; // FanControl_t
   Erd_t ambientFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t ambientFilteredHumidityPercentx100ResolvedErd; // RelativeHumidityPercentx100_t
   Erd_t ambientThermistorIsValidErd; // bool
   Erd_t ambientHumiditySensorIsValidErd; // bool
   Erd_t fanAntiSweatBehaviorEnabledErd; // bool
} FanSpeedResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FanSpeedResolverConfig_t *config;
      const FanData_t *fanData;
      EventSubscription_t dataModelSubscription;
   } _private;
} FanSpeedResolver_t;

/*!
 * @param instance
 * @param dataModel
 * @param fanData
 * @param config
 */
void FanSpeedResolver_Init(
   FanSpeedResolver_t *instance,
   I_DataModel_t *dataModel,
   const FanData_t *fanData,
   const FanSpeedResolverConfig_t *config);

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONDENSERFANANTISWEATBEHAVIOR_H
#define CONDENSERFANANTISWEATBEHAVIOR_H

#include "FanData.h"
#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   Erd_t ambientFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t ambientFilteredHumidityPercentx100ResolvedErd; // RelativeHumidityPercentx100_t
   Erd_t condenserFanAntiSweatBehaviorVoteErd; // FanVotedSpeed_t
   Erd_t ambientTemperatureSensorIsValidErd; // bool
   Erd_t ambientHumiditySensorIsValidErd; // bool
   Erd_t condenserFanAntiSweatBehaviorEnabledByGridErd; // bool
   Erd_t coolingModeErd; // CoolingMode_t
} CondenserFanAntiSweatBehaviorConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;

      const CondenserFanAntiSweatBehaviorConfig_t *config;
      const FanData_t *fanData;
   } _private;
} CondenserFanAntiSweatBehavior_t;

void CondenserFanAntiSweatBehavior_Init(
   CondenserFanAntiSweatBehavior_t *instance,
   I_DataModel_t *dataModel,
   const CondenserFanAntiSweatBehaviorConfig_t *config,
   const FanData_t *fanData);

#endif

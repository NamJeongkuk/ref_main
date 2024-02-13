/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENHANCEDSABBATHMODE_H
#define ENHANCEDSABBATHMODE_H

#include "I_DataModel.h"
#include "Timer.h"
#include "Hsm.h"
#include "EnhancedSabbathData.h"
#include "LightingData.h"
#include "ErdList.h"

typedef struct
{
   Erd_t disableMinimumCompressorTimesVoteErd; // BooleanVotedState_t
   Erd_t compressorSpeedVoteErd; // CompressorVotedSpeed_t
   Erd_t condenserFanVoteErd; // FanVotedSpeed_t
   Erd_t freezerEvapFanVoteErd; // FanVotedSpeed_t
   Erd_t damperPositionVoteErd; // DamperVotedPosition_t
   Erd_t enhancedSabbathModeStatusErd; // bool
   Erd_t regularSabbathModeStatusErd; // bool
   Erd_t freshFoodSetpointVoteErd; // SetpointVotedTemperature_t
   Erd_t freezerSetpointVoteErd; // SetpointVotedTemperature_t
   Erd_t freshFoodAverageCabinetTemperatureErd; // TemperatureDegFx100_t
   Erd_t freezerAverageCabinetTemperatureErd; // TemperatureDegFx100_t
   Erd_t freshFoodCabinetSetpointErd; // TemperatureDegFx100_t
   Erd_t freezerCabinetSetpointErd; // TemperatureDegFx100_t
   Erd_t dispensingDisabledErd; // bool
   Erd_t waitingToDefrostErd; // bool
   Erd_t gridOverrideEnabledErd; // bool
   Erd_t coolingModeErd; // CoolingMode_t
   Erd_t hsmStateErd; // EnhancedSabbathModeHsmState_t
   Erd_t defrostingErd; // bool
   Erd_t enhancedSabbathIsRequestingDefrostErd; // bool
   Erd_t enhancedSabbathStageFreshFoodCoolingIsActiveErd; // bool
   Erd_t enhancedSabbathStageFreezerCoolingIsActiveErd; // bool
   Erd_t sabbathIsReadyToDefrostErd; // bool
   Erd_t enhancedSabbathRunTimeInMinutesErd; // uint16_t
   ErdList_t lightVoteErdList; // ErdList_t
} EnhancedSabbathModeConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      TimerModule_t *timerModule;
      const EnhancedSabbathModeConfig_t *config;
      const EnhancedSabbathData_t *enhancedSabbathData;
      EventSubscription_t dataModelSubscription;
      Timer_t stageTimer;
      Timer_t enhancedSabbathModeRunTimeTimer;
      Hsm_t hsm;
   } _private;
} EnhancedSabbathMode_t;

void EnhancedSabbathMode_Init(
   EnhancedSabbathMode_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const EnhancedSabbathData_t *enhancedSabbathData,
   const EnhancedSabbathModeConfig_t *config);

#endif

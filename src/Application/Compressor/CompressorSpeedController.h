/*!
 * @file
 * @brief Controls Compressor Speed Frequency
 *
 * This module controls and manages the requests to the lower level compressor. It enforces minimum times
 * as well as startup conditions.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSPEEDCONTROLLER_H
#define COMPRESSORSPEEDCONTROLLER_H

#include "Timer.h"
#include "I_DataModel.h"
#include "CompressorData.h"
#include "CompressorVotedSpeed.h"
#include "Hsm.h"

typedef struct
{
   Erd_t compressorStateErd; // CompressorState_t
   Erd_t compressorSpeedRequestErd; // CompressorSpeed_t
   Erd_t compressorSpeedResolvedVoteErd; // CompressorVotedSpeed_t
   Erd_t valvePositionResolvedVoteErd; // VotedValvePosition_t
   Erd_t filteredAmbientTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t disableMinimumCompressorTimeErd; // bool
   uint8_t sabbathDelayTimeInSeconds; // uint8_t
} CompressorSpeedControllerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const CompressorData_t *compressorData;
      const CompressorSpeedControllerConfiguration_t *config;
      Timer_t compressorTimer;
      Timer_t sabbathDelayTimer;
      Timer_t startupTimer;
      Timer_t minimumRunTimer;
      Timer_t remainOffAfterValveMoveTimer;
      TimerModule_t *timerModule;
      Hsm_t hsm;
      CompressorSpeed_t cacheSpeedFromResolvedVote;
      EventSubscription_t dataModelChangedSubscription;
   } _private;
} CompressorSpeedController_t;

/*!
 * @param instance the compressor speed controller instance
 * @param dataModel
 * @param config the compressor speed controller configuration
 */
void CompressorSpeedController_Init(
   CompressorSpeedController_t *instance,
   I_DataModel_t *dataModel,
   const CompressorSpeedControllerConfiguration_t *config);

#endif

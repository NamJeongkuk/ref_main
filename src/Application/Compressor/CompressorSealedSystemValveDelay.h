/*!
 * @file
 * @brief This module handles voting for the compressor and evap fans whenever the sealed system valve changes position and the compressor is off and under grid control.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSEALEDSYSTEMVALVEDELAY_H
#define COMPRESSORSEALEDSYSTEMVALVEDELAY_H

#include "I_DataModel.h"
#include "CompressorData.h"
#include "EventSubscription.h"
#include "Erd.h"
#include "Timer.h"
#include "Fsm.h"
#include "ErdList.h"

typedef struct
{
   Erd_t gridCompressorVoteErd; // CompressorVotedSpeed_t
   Erd_t winningVoteErd; // Erd_t
   Erd_t ambientTemperatureIsValidErd; // bool
   Erd_t ambientTemperatureErd; // TemperatureDegFx100_t
   Erd_t sealedSystemValveVotedPositionErd; // SealedSystemValvePosition_t
   Erd_t compressorDelayVoteErd; // CompressorVotedSpeed_t
   ErdList_t fanDelayVotesErdList; // FanVotedSpeed_t
} CompressorSealedSystemValveDelayConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      Timer_t timer;
      I_DataModel_t *dataModel;
      const CompressorData_t *data;
      const CompressorSealedSystemValveDelayConfiguration_t *config;
   } _private;
} CompressorSealedSystemValveDelay_t;

void CompressorSealedSystemValveDelay_Init(
   CompressorSealedSystemValveDelay_t *instance,
   I_DataModel_t *dataModel,
   const CompressorData_t *data,
   const CompressorSealedSystemValveDelayConfiguration_t *config);

#endif

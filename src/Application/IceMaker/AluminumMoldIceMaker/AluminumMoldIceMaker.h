/*!
 * @file
 * @brief Aluminum mold ice maker state machine
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKER_H
#define ALUMINUMMOLDICEMAKER_H

#include "I_DataModel.h"
#include "AluminumMoldIceMakerData.h"
#include "Hsm.h"

typedef struct
{
   Erd_t aluminumMoldIceMakerHsmStateErd; // AluminumMoldIceMakerHsmState_t
   Erd_t waterValveVoteErd; // WaterValveVotedState_t
   Erd_t iceMakerHeaterVoteErd; // HeaterVotedState_t
   Erd_t iceMakerMotorVoteErd; // AluminumMoldIceMakerMotorState_t
   Erd_t harvestCountCalculationRequestErd; // bool
   Erd_t feelerArmMonitoringRequestErd; // bool
   Erd_t harvestCountIsReadyToHarvestErd; // bool
   Erd_t iceMakerFilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t feelerArmIsReadyToEnterHarvestErd; // bool
   Erd_t iceMakerEnabledErd; // bool
   Erd_t sabbathModeErd; // bool
} AluminumMoldIceMakerConfig_t;

typedef struct
{
   struct
   {
      Hsm_t hsm;
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const AluminumMoldIceMakerConfig_t *config;
      const AluminumMoldIceMakerData_t *iceMakerParametricData;
   } _private;
} AluminumMoldIceMaker_t;

void AluminumMoldIceMaker_Init(
   AluminumMoldIceMaker_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerConfig_t *config);

#endif
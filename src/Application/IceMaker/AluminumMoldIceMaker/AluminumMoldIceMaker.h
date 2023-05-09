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
#include "Timer.h"

typedef struct
{
   Erd_t aluminumMoldIceMakerHsmStateErd; // AluminumMoldIceMakerHsmState_t
   Erd_t iceMakerWaterValveVoteErd; // WaterValveVotedState_t
   Erd_t iceMakerHeaterVoteErd; // HeaterVotedState_t
   Erd_t iceMakerMotorVoteErd; // AluminumMoldIceMakerMotorVotedState_t
   Erd_t harvestCountCalculationRequestErd; // bool
   Erd_t feelerArmMonitoringRequestErd; // bool
   Erd_t harvestCountIsReadyToHarvestErd; // bool
   Erd_t iceMakerFilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t feelerArmIsReadyToEnterHarvestErd; // bool
   Erd_t iceMakerEnabledErd; // bool
   Erd_t sabbathModeErd; // bool
   Erd_t fillTubeHeaterVoteErd; // PercentageDutyCycleVote_t
   Erd_t moldHeaterControlRequestErd; // IceMakerMoldHeaterControlRequest_t
   Erd_t rakeCompletedRevolutionErd; // bool
   Erd_t moldThermistorIsValidErd; // bool
   Erd_t skipFillRequestErd; // bool
   Erd_t rakeControlRequestErd; // bool
   Erd_t isolationWaterValveVoteErd; // WaterValveVotedState_t
   Erd_t waterFillMonitoringRequestErd; // IceMakerWaterFillMonitoringRequest_t
   Erd_t stopFillSignalErd; // Signal_t
   Erd_t rakePositionErd; // RakePosition_t
   Erd_t freezerIceRateTriggerSignalErd; // Signal_t
   Erd_t aluminumMoldIceMakerTestRequestErd; // AluminumMoldIceMakerTestRequest_t
   Erd_t dispensingRequestStatusErd; // DispensingRequestStatus_t
} AluminumMoldIceMakerConfig_t;

typedef struct
{
   struct
   {
      Hsm_t hsm;
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      Timer_t fillTubeHeaterTimer;
      Timer_t maxHarvestTimer;
      Timer_t maxHarvestFixTimer;
      Timer_t minimumHeaterOnTimer;
      Timer_t harvestFaultMaxTimer;
      Timer_t rakeMotorControlTimer;
      const AluminumMoldIceMakerConfig_t *config;
      const AluminumMoldIceMakerData_t *iceMakerParametricData;
      bool initialFreezeStateTransition;
      bool revolutionCompletedDuringHarvestFix;
      bool pauseFillMonitoring;
   } _private;
} AluminumMoldIceMaker_t;

void AluminumMoldIceMaker_Init(
   AluminumMoldIceMaker_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerConfig_t *config);

#endif

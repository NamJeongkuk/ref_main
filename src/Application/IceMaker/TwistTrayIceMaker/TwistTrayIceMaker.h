/*!
 * @file This is the main state machine that drives the twist ice maker logic
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKER_H
#define TWISTTRAYICEMAKER_H

#include "Timer.h"
#include "I_DataSource.h"
#include "Fsm.h"
#include "TwistTrayIceMakerData.h"

typedef struct
{
   Erd_t highLevelStateErd; // uint8_t
   Erd_t operationStateErd; // uint8_t
   Erd_t thermistorIsValidResolvedErd; // bool
   Erd_t filteredTemperatureResolvedInDegFx100Erd; // int16_t
   Erd_t testRequestErd; // uint8_t
   Erd_t stopFillSignalErd; // uint8_t
   Erd_t harvestCountIsReadyToHarvestErd; // bool
   Erd_t harvestCountCalculationRequestErd; // bool
   Erd_t motorIceMakerVoteErd; // uint8_t vote
   Erd_t waterValveIceMakerVoteErd; // uint8_t vote
   Erd_t motorActionResultErd; // uint8_t
   Erd_t motorFaultActiveErd; // bool
   Erd_t waterFillMonitoringRequestErd; // uint8_t
   Erd_t isolationWaterValveVoteErd; // uint8_t vote
   Erd_t iceMakerEnabledResolvedErd; // bool
   Erd_t sabbathModeErd; // bool
   Erd_t enhancedSabbathModeErd; // bool
   Erd_t freezerIceRateTriggerSignalErd; // uint8_t
   Erd_t fillTubeHeaterVoteErd; // uint8_t
   Erd_t coolingOffStatusErd; // bool
   Erd_t freezerIceRateIsActiveErd; // bool
   Erd_t dispensingRequestStatusErd; // DispensingRequestStatus_t
   Erd_t leftSideFreezerDoorStatusResolvedErd; // bool
   Erd_t dispensingInhibitedErd; // DispensingInhibitedReasonBitmap_t
} TwistTrayIceMakerConfiguration_t;

typedef struct
{
   struct
   {
      TimerModule_t *timerModule;
      I_DataSource_t *dataSource;

      EventSubscription_t dataSourceChangeEventSubscription;

      Timer_t waitingTimer;
      Timer_t doorHarvestDelayTimer;
      Timer_t fillTubeHeaterTimer;
      Timer_t dispensingIceTimer;
      Timer_t retryMotorInitTimer;
      Timer_t doorOpenCheckTimer;

      bool firstFreezeTransition;
      bool doorHarvestDelayHasElapsed;
      bool iceDispensedLongEnoughToCheckHarvest;
      bool pauseFillMonitoring;
      bool delayFillMonitoring;
      bool doorOpenCheckTimeElapsed;

      const TwistTrayIceMakerData_t *parametric;
      const TwistTrayIceMakerConfiguration_t *config;

      Fsm_t fsm;
   } _private;
} TwistTrayIceMaker_t;

/*!
 * Initializes the Twist Tray Ice Maker
 * @param instance
 * @param timerModule the system timer module
 * @param dataSource the system data source
 * @param parametric
 */
void TwistTrayIceMaker_Init(
   TwistTrayIceMaker_t *instance,
   TimerModule_t *timerModule,
   I_DataSource_t *dataSource,
   const TwistTrayIceMakerConfiguration_t *config,
   const TwistTrayIceMakerData_t *parametric);

#endif

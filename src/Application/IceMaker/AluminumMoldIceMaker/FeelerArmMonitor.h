/*!
 * @file
 * @brief The module starts feeler arm position monitoring when requested.
 * It sets feeler arm is ready to enter harvest after timeout when bucket is empty and monitoring is requested
 * And, clear when else conditions.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEELERARMMONITOR_H
#define FEELERARMMONITOR_H

#include "EventSubscription.h"
#include "AluminumMoldIceMakerData.h"
#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t feelerArmMonitoringRequestErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
   Erd_t feelerArmIsReadyToEnterHarvestErd; // bool
   Erd_t feelerArmPositionErd; // FeelerArmPosition_t
} FeelerArmMonitorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t feelerArmMonitoringRequestSubscription;
      EventSubscription_t feelerArmPositionSubscription;
      Timer_t feelerArmDelayTimer;
      const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
      const FeelerArmMonitorConfig_t *config;
   } _private;
} FeelerArmMonitor_t;

void FeelerArmMonitor_Init(
   FeelerArmMonitor_t *instance,
   I_DataModel_t *dataModel,
   const FeelerArmMonitorConfig_t *config);

#endif

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
   struct
   {
      TimerModule_t *timerModule;
      I_DataSource_t *dataSource;

      EventSubscription_t dataSourceChangeEventSubscription;

      Timer_t minimumFreezeTimer;
      Timer_t integrationTimer;
      Timer_t waitingTimer;
      Timer_t doorClosedTimer;
      Timer_t dispensingIceTimer;

      bool firstFreezeTransition;
      bool minimumFreezeTimeReached : 1;
      bool freezeIntegrationSumReached : 1;
      bool doorHasBeenClosedForLongEnough : 1;
      bool maximumHarvestTemperatureReached : 1;
      bool iceDispensedLongEnoughToCheckHarvest;
      uint8_t unused : 4;

      const TwistTrayIceMakerData_t *parametric;

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
   const TwistTrayIceMakerData_t *parametric);

#endif

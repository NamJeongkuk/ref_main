/*!
 * @file
 * @brief This is a temporary module to monitor the switch input. It will be replaced once the interrupt driven debouncer is merged
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORSWITCHMONITOR_H
#define TWISTTRAYICEMAKERMOTORSWITCHMONITOR_H

#include "EventSubscription.h"
#include "Erd.h"
#include "TwistTrayIceMakerMotorController.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t switchInputErd; // bool
} TwistTrayIceMakerMotorSwitchMonitorConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t switchInputSubscription;
      const TwistTrayIceMakerMotorSwitchMonitorConfig_t *config;
      TwistTrayIceMakerMotorController_t *motorController;
   } _private;
} TwistTrayIceMakerMotorSwitchMonitor_t;

void TwistTrayIceMakerMotorSwitchMonitor_Init(
   TwistTrayIceMakerMotorSwitchMonitor_t *instance,
   I_DataModel_t *dataModel,
   TwistTrayIceMakerMotorController_t *motorController,
   const TwistTrayIceMakerMotorSwitchMonitorConfig_t *config);

#endif

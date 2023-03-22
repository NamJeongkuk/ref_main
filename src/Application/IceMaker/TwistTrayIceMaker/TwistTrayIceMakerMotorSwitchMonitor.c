/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorSwitchMonitor.h"
#include "utils.h"
#include "SystemErds.h"

static void OnInputChange(void *context, const void *args)
{
   TwistTrayIceMakerMotorSwitchMonitor_t *instance = context;
   const bool *newState = args;

   TwistTrayIceMakerMotorController_UpdateSwitchState(
      instance->_private.motorController,
      !(*newState));
}

void TwistTrayIceMakerMotorSwitchMonitor_Init(
   TwistTrayIceMakerMotorSwitchMonitor_t *instance,
   I_DataModel_t *dataModel,
   TwistTrayIceMakerMotorController_t *motorController,
   const TwistTrayIceMakerMotorSwitchMonitorConfig_t *config)
{
   instance->_private.motorController = motorController;
   instance->_private.config = config;

   bool initialState;
   DataModel_Read(dataModel, instance->_private.config->switchInputErd, &initialState);
   TwistTrayIceMakerMotorController_UpdateSwitchState(
      instance->_private.motorController,
      !(initialState));

   EventSubscription_Init(
      &instance->_private.switchInputSubscription,
      instance,
      OnInputChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->switchInputErd,
      &instance->_private.switchInputSubscription);
}

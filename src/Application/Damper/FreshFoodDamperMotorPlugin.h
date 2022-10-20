/*!
 * @file
 * @brief plugin for fresh food damper motor
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODDAMPERMOTORPLUGIN_H
#define FRESHFOODDAMPERMOTORPLUGIN_H

#include "I_DataModel.h"
#include "StepperMotorDriver.h"
#include "FreshFoodDamperRequestManager.h"
#include "DamperMaxOpenTimeMonitor.h"
#include "SingleDamperData.h"
#include "Timer.h"
#include "ErdResolver.h"
#include "Event_Synchronous.h"

typedef struct
{
   struct
   {
      StepperMotorDriver_t stepperMotorDriver;
      FreshFoodDamperRequestManager_t damperRequestManager;
      DamperMaxOpenTime_t damperMaxOpenTime;
      Event_Synchronous_t damperStepEvent;
      Timer_t stepEventTimer;
      ErdResolver_t damperErdResolver;
   } _private;
} FreshFoodDamperMotorPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreshFoodDamperMotorPlugin_Init(FreshFoodDamperMotorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
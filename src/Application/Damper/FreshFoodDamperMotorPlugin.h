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
#include "DamperRequestManager.h"
#include "DamperHeaterDefrostControl.h"
#include "DamperMaxOpenTimeMonitor.h"
#include "DamperFreezePrevention.h"
#include "SingleDamperData.h"
#include "Timer.h"
#include "ErdResolver.h"
#include "Event_OneMillisecondTimer.h"

typedef struct
{
   struct
   {
      StepperMotorDriver_t stepperMotorDriver;
      DamperRequestManager_t damperRequestManager;
      DamperFreezePrevention_t damperFreezePrevention;
      DamperHeaterDefrostControl_t freshFoodDamperHeaterDefrostControl;
      DamperMaxOpenTime_t damperMaxOpenTime;
      ErdResolver_t damperPositionErdResolver;
      ErdResolver_t damperHeaterErdResolver;
   } _private;
} FreshFoodDamperMotorPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreshFoodDamperMotorPlugin_Init(FreshFoodDamperMotorPlugin_t *instance, I_DataModel_t *dataModel);

#endif

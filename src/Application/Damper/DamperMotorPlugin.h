/*!
 * @file
 * @brief plugin for damper motor
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERMOTORPLUGIN_H
#define DAMPERMOTORPLUGIN_H

#include "I_DataModel.h"
#include "StepperMotorDriver.h"

typedef struct
{
   struct
   {
      StepperMotorDriver_t stepperMotorDriver;
   } _private;
} DamperMotorPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void DamperMotorPlugin_Init(DamperMotorPlugin_t *instance, I_DataModel_t *dataModel);

#endif

/*!
 * @file
 * @brief Side by Side Parallel Motor Control Plugin
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEPARALLELMOTORCONTROLPLUGIN_H
#define SIDEBYSIDEPARALLELMOTORCONTROLPLUGIN_H

#include "I_DataModel.h"
#include "MotorDriverArbitrator.h"

typedef struct
{
   struct
   {
      MotorDriverArbitrator_t motorDriverArbitrator;
   } _private;
} SideBySideParallelMotorControlPlugin_t;

void SideBySideParallelMotorControlPlugin_Init(
   SideBySideParallelMotorControlPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif

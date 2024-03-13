/*!
 * @file
 * @brief Parallel Motor Control Plugin
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PARALLELMOTORCONTROLPLUGIN_H
#define PARALLELMOTORCONTROLPLUGIN_H

#include "I_DataModel.h"
#include "MotorDriverArbitrator.h"
#include "ErdLogicService.h"

typedef struct
{
   struct
   {
      MotorDriverArbitrator_t motorDriverArbitrator;
      ErdLogicService_t erdLogicService;
   } _private;
} ParallelMotorControlPlugin_t;

void ParallelMotorControlPlugin_Init(
   ParallelMotorControlPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif

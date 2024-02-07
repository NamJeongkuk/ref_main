/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef THREEWAYSEALEDSYSTEMVALVEPLUGIN_H
#define THREEWAYSEALEDSYSTEMVALVEPLUGIN_H

#include "StepperMotorDriver.h"
#include "I_DataModel.h"
#include "SealedSystemValveRequestManager.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      StepperMotorDriver_t stepperMotorDriver;
      SealedSystemValveRequestManager_t requestManager;
      ErdResolver_t positionErdResolver;
   } _private;
} ThreeWaySealedSystemValvePlugin_t;

void ThreeWaySealedSystemValvePlugin_Init(ThreeWaySealedSystemValvePlugin_t *instance, I_DataModel_t *dataModel);

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerRunner.h"
#include "utils.h"

static void OnInterrupt(void *context, const void *args)
{
   IGNORE(args);
   TwistTrayIceMakerRunner_t *instance = context;

   TwistTrayIceMakerMotorController_Run(instance->_private.motorController);
}

void TwistTrayIceMakerRunner_Init(
   TwistTrayIceMakerRunner_t *instance,
   TwistTrayIceMakerMotorController_t *motorController,
   I_Interrupt_t *interrupt)
{
   instance->_private.motorController = motorController;

   EventSubscription_Init(&instance->_private.on1MsInterruptSubscription, instance, OnInterrupt);
   Event_Subscribe(interrupt->OnInterrupt, &instance->_private.on1MsInterruptSubscription);
}

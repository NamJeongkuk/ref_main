/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERRUNNER_H
#define TWISTTRAYICEMAKERRUNNER_H

#include "EventSubscription.h"
#include "TwistTrayIceMakerMotorController.h"
#include "I_Interrupt.h"

typedef struct
{
   struct
   {
      EventSubscription_t on1MsInterruptSubscription;
      TwistTrayIceMakerMotorController_t *motorController;
   } _private;
} TwistTrayIceMakerRunner_t;

void TwistTrayIceMakerRunner_Init(
   TwistTrayIceMakerRunner_t *instance,
   TwistTrayIceMakerMotorController_t *motorController,
   I_Interrupt_t *interrupt);

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTORDOACTION_H
#define ICEMAKERMOTORDOACTION_H

#include "IceMakerMotorAction.h"
#include "Signal.h"

typedef struct
{
   IceMakerMotorAction_t action;
   Signal_t signal;
} IceMakerMotorDoAction_t;

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERALLOWABLEINTERIORLIGHTSTATE_H
#define USERALLOWABLEINTERIORLIGHTSTATE_H

#include "PercentageDutyCycle.h"

typedef struct
{
   bool userAllowable;
   PercentageDutyCycle_t percentageDutyCycle;
} UserAllowableInteriorLightState_t;

#endif

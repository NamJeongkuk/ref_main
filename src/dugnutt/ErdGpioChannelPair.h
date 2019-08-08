/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ERDGPIOCHANNELPAIR_H
#define ERDGPIOCHANNELPAIR_H

#include "Erd.h"
#include "I_GpioGroup.h"

typedef struct
{
   Erd_t erd;
   GpioChannel_t channel;
} ErdGpioChannelPair_t;

#endif

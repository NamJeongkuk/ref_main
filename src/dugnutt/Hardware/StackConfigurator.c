/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TDD_BUILD

#include "StackConfigurator.h"
#include "StackDirection.h"
#include <stdint.h>

static const uint8_t pattern[4] = {0xDE, 0xAD, 0xBE, 0xEF};
extern char _stackCheckEnd;
extern char _iStackSize;

void StackConfigurator_GetConfiguration(StackConfiguration_t *stackConfiguration)
{
   stackConfiguration->stackSize = (uintptr_t)&_iStackSize;
   stackConfiguration->stackStartAddress = (uint8_t *)&_stackCheckEnd;
   stackConfiguration->pattern = pattern;
   stackConfiguration->stackDirection = StackDirection_ReverseGrowing;
   stackConfiguration->patternSize = sizeof(pattern);
}

#endif

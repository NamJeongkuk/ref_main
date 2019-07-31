/*!
 * @file
 * @brief This module sets the platform specific stack information for other modules.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TDD_BUILD

#include "StackConfigurator.h"
#include "StackDirection.h"
#include <stdint.h>

static const uint8_t pattern[4] = {0xDE, 0xAD, 0xBE, 0xEF};
extern char istack;
extern char _iStackSize;

void StackConfigurator_GetConfiguration(StackConfiguration_t *stackConfiguration)
{
   stackConfiguration->stackStartAddress = (uint8_t *)&istack;
   stackConfiguration->stackSize = (uintptr_t)&_iStackSize;
   stackConfiguration->stackDirection = StackDirection_ReverseGrowing;
   stackConfiguration->pattern = pattern;
   stackConfiguration->patternSize = sizeof(pattern);
}

#endif

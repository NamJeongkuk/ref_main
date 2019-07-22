/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplicationResetSetup.h"

#include <stdint.h>
#include "StackConfigurator.h"
#include "StackConfiguration.h"
#include "InitializeStackMemory.h"
#include "ClockConfig.h"

void ApplicationResetSetup(void)
{
   ClockConfig_SetHOCO(U32_SCKCR_REGISTER_MASK_HIGHSPEED);

   StackConfiguration_t stackConfiguration;
   uint8_t *stackStartAddress;
   uint32_t stackSize;
   StackDirection_t stackDirection;
   const uint8_t *pattern;
   uint8_t patternSize;

   StackConfigurator_GetConfiguration(&stackConfiguration);
   stackStartAddress = stackConfiguration.stackStartAddress;
   stackSize = stackConfiguration.stackSize;
   stackDirection = stackConfiguration.stackDirection;
   pattern = stackConfiguration.pattern;
   patternSize = stackConfiguration.patternSize;

   INIT_STACK_MEMORY(stackStartAddress, stackSize, stackDirection, pattern, patternSize);

   __builtin_rx_setpsw('I');
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TDD_BUILD

#include "StackConfigurator.h"
#include "StackDirection.h"
#include "bsp_cfg.h"
#include <stdint.h>

static const uint8_t pattern[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
extern char __StackSize;
extern char __stackCheckEnd;

static const StackConfiguration_t stackConfiguration = {
   BSP_CFG_STACK_MAIN_BYTES,
   (uint8_t *)&__stackCheckEnd,
   pattern,
   StackDirection_ReverseGrowing,
   sizeof(pattern)
};

const StackConfiguration_t *StackConfigurator_GetConfiguration(void)
{
   return &stackConfiguration;
}

#endif
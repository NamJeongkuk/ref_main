/*!
 * @file
 * @brief Initialize ErdResolvers for setpoint ERDs
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESETPOINTRESOLVERPLUGIN_H
#define SIDEBYSIDESETPOINTRESOLVERPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      ErdResolver_t freshFoodSetpointResolver;
      ErdResolver_t freezeSetpointResolver;
   } _private;
} SideBySideSetpointResolverPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void SideBySideSetpointResolverPlugin_Init(
   SideBySideSetpointResolverPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif

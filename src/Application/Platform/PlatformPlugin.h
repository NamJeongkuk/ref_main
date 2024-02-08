/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PLATFORMPLUGIN_H
#define PLATFORMPLUGIN_H

#include "SideBySideConfigurationPlugin.h"
#include "FourDoorConfigurationPlugin.h"
#include "I_DataModel.h"
#include "SingleDoorFreshFoodConfigurationPlugin.h"
#include "SingleDoorFreezerConfigurationPlugin.h"

typedef struct
{
   struct
   {
      union
      {
         SideBySideConfigurationPlugin_t sideBySidePlugin;
         FourDoorConfigurationPlugin_t fourDoorPlugin;
         SingleDoorFreshFoodConfigurationPlugin_t singleDoorFreshFoodPlugin;
         SingleDoorFreezerConfigurationPlugin_t singleDoorFreezerPlugin;
      } platformConfigurations;
   } _private;
} PlatformPlugin_t;

void PlatformPlugin_Init(PlatformPlugin_t *instance, I_DataModel_t *dataModel);

#endif

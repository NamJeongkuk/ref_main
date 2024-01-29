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

typedef struct
{
   struct
   {
      union
      {
         SideBySideConfigurationPlugin_t sideBySidePlugin;
         FourDoorConfigurationPlugin_t fourDoorPlugin;
      } platformConfigurations;
   } _private;
} PlatformPlugin_t;

void PlatformPlugin_Init(PlatformPlugin_t *instance, I_DataModel_t *dataModel);

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERSLOTSPLUGIN_H
#define ICEMAKERSLOTSPLUGIN_H

#include "I_DataModel.h"
#include "AluminumMoldIceMakerConfigurator.h"
#include "TwistTrayIceMakerConfigurator.h"

typedef struct
{
   struct
   {
      union
      {
         TwistTrayIceMakerConfigurator_t twistTrayIceMakerConfigurator;
         AluminumMoldIceMakerConfigurator_t aluminumMoldIceMakerConfigurator;
      } slot0;
      union
      {
         TwistTrayIceMakerConfigurator_t twistTrayIceMakerConfigurator;
         AluminumMoldIceMakerConfigurator_t aluminumMoldIceMakerConfigurator;
      } slot1;
      union
      {
         TwistTrayIceMakerConfigurator_t twistTrayIceMakerConfigurator;
         AluminumMoldIceMakerConfigurator_t aluminumMoldIceMakerConfigurator;
      } slot2;
   } _private;
} IceMakerSlotsPlugin_t;

void IceMakerSlotsPlugin_Init(IceMakerSlotsPlugin_t *instance, I_DataModel_t *dataModel);

#endif

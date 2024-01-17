/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERCONFIGURATOR_H
#define TWISTTRAYICEMAKERCONFIGURATOR_H

#include "TwistTrayIceMakerPlugin.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      TwistTrayIceMakerPlugin_t plugin;
   } _private;
} TwistTrayIceMakerConfigurator_t;

void TwistTrayIceMaker0Configurator_Init(TwistTrayIceMakerConfigurator_t *instance, I_DataModel_t *dataModel);
void TwistTrayIceMaker1Configurator_Init(TwistTrayIceMakerConfigurator_t *instance, I_DataModel_t *dataModel);
void TwistTrayIceMaker2Configurator_Init(TwistTrayIceMakerConfigurator_t *instance, I_DataModel_t *dataModel);

#endif

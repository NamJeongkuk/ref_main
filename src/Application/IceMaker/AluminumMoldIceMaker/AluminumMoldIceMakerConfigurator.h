/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERCONFIGURATOR_H
#define ALUMINUMMOLDICEMAKERCONFIGURATOR_H

#include "AluminumMoldIceMakerPlugin.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      AluminumMoldIceMakerPlugin_t plugin;
   } _private;
} AluminumMoldIceMakerConfigurator_t;

void AluminumMoldIceMaker0Configurator_Init(AluminumMoldIceMakerConfigurator_t *instance, I_DataModel_t *dataModel);
void AluminumMoldIceMaker1Configurator_Init(AluminumMoldIceMakerConfigurator_t *instance, I_DataModel_t *dataModel);
void AluminumMoldIceMaker2Configurator_Init(AluminumMoldIceMakerConfigurator_t *instance, I_DataModel_t *dataModel);

#endif

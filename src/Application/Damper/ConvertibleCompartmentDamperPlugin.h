/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTDAMPERPLUGIN_H
#define CONVERTIBLECOMPARTMENTDAMPERPLUGIN_H

#include "I_DataModel.h"
#include "ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin.h"

typedef struct
{
   struct
   {
      ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_t votingFramework;
   } _private;
} ConvertibleCompartmentDamperPlugin_t;

void ConvertibleCompartmentDamperPlugin_Init(
   ConvertibleCompartmentDamperPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTSTATEPLUGIN_H
#define CONVERTIBLECOMPARTMENTSTATEPLUGIN_H

#include "ConvertibleCompartmentState.h"
#include "ConvertibleCompartmentData.h"
#include "I_DataModel.h"

typedef struct
{
   ConvertibleCompartmentState_t instance;
} ConvertibleCompartmentStatePlugin_t;

void ConvertibleCompartmentStatePlugin_Init(ConvertibleCompartmentStatePlugin_t *instance, I_DataModel_t *dataModel);

#endif

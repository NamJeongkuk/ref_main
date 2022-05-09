/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTCABINETSTATEPLUGIN_H
#define CONVERTIBLECOMPARTMENTCABINETSTATEPLUGIN_H

#include "ConvertibleCompartmentCabinetState.h"
#include "ConvertibleCompartmentCabinetData.h"
#include "I_DataModel.h"

typedef struct
{
   ConvertibleCompartmentCabinetState_t instance;
} ConvertibleCompartmentCabinetStatePlugin_t;

void ConvertibleCompartmentCabinetStatePlugin_Init(ConvertibleCompartmentCabinetStatePlugin_t *instance, I_DataModel_t *dataModel);

#endif

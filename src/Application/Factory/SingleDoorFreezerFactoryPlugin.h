/*!
 * @file
 * @brief plugin for Single Door Freezer Factory
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERFACTORYPLUGIN_H
#define SINGLEDOORFREEZERFACTORYPLUGIN_H

#include "I_DataModel.h"
#include "FactoryMode.h"

typedef struct
{
   struct
   {
      FactoryMode_t factoryMode;
   } _private;
} SingleDoorFreezerFactoryPlugin_t;

/*!
 * Initialize Single Door Freezer Factory Plugin
 * @param instance
 * @param dataModel
 */
void SingleDoorFreezerFactoryPlugin_Init(SingleDoorFreezerFactoryPlugin_t *instance, I_DataModel_t *dataModel);

#endif

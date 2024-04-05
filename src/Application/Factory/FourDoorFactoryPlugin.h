/*!
 * @file
 * @brief plugin for Four Door Factory
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORFACTORYPLUGIN_H
#define FOURDOORFACTORYPLUGIN_H

#include "I_DataModel.h"
#include "FactoryMode.h"

typedef struct
{
   struct
   {
      FactoryMode_t factoryMode;
   } _private;
} FourDoorFactoryPlugin_t;

/*!
 * Initialize Four Door Factory Plugin
 * @param instance
 * @param dataModel
 */
void FourDoorFactoryPlugin_Init(FourDoorFactoryPlugin_t *instance, I_DataModel_t *dataModel);

#endif

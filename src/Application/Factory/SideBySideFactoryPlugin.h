/*!
 * @file
 * @brief plugin for SxS Factory
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEFACTORYPLUGIN_H
#define SIDEBYSIDEFACTORYPLUGIN_H

#include "I_DataModel.h"
#include "FactoryMode.h"

typedef struct
{
   struct
   {
      FactoryMode_t factoryMode;
   } _private;
} SideBySideFactoryPlugin_t;

/*!
 * Initialize SxS Factory Plugin
 * @param instance
 * @param dataModel
 */
void SideBySideFactoryPlugin_Init(SideBySideFactoryPlugin_t *instance, I_DataModel_t *dataModel);

#endif

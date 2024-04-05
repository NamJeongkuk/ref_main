/*!
 * @file
 * @brief plugin for Single Door Fresh Food Factory
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODFACTORYPLUGIN_H
#define SINGLEDOORFRESHFOODFACTORYPLUGIN_H

#include "I_DataModel.h"
#include "FactoryMode.h"

typedef struct
{
   struct
   {
      FactoryMode_t factoryMode;
   } _private;
} SingleDoorFreshFoodFactoryPlugin_t;

/*!
 * Initialize Single Door Fresh Food Factory Plugin
 * @param instance
 * @param dataModel
 */
void SingleDoorFreshFoodFactoryPlugin_Init(SingleDoorFreshFoodFactoryPlugin_t *instance, I_DataModel_t *dataModel);

#endif

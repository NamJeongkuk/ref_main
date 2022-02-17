/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDPLUGIN_H
#define GRIDPLUGIN_H

#include "Grid.h"
#include "I_DataModel.h"

typedef struct
{
   Grid_t gridInstance;
} GridPlugin_t;

void GridPlugin_Init(GridPlugin_t *instance, I_DataModel_t *dataModel);

#endif

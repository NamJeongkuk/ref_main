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
#include "RingBuffer.h"
#include "GridLineCalculator.h"
#include "GridBlockCalculator.h"

typedef struct
{
   Grid_t gridInstance;
   GridLineCalculator_t gridLineCalculator;
   GridBlockCalculator_t gridBlockCalculator;
} GridPlugin_t;

void GridPlugin_Init(
   GridPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif

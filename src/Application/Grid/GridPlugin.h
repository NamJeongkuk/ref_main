/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDPLUGIN_H
#define GRIDPLUGIN_H

#include "Grid.h"
#include "CalcGridBlockAndGridLines.h"
#include "I_DataModel.h"
#include "RingBuffer.h"

typedef struct
{
   Grid_t gridInstance;
   RingBuffer_t bufferInstance;
   CalcGridBlockAndLines_t calcGridBlockAndGridLinesInstance;
} GridPlugin_t;

void GridPlugin_Init(GridPlugin_t *instance, I_DataModel_t *dataModel);

#endif

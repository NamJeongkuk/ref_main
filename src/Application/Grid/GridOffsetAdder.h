/*!
 * @file
 * @brief Adds together Cabinet Offset and Cross Ambient Offset Erds and produces result erds
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDOFFSETADDER_H
#define GRIDOFFSETADDER_H

#include "I16ErdAdder.h"
#include "SystemErds.h"

typedef struct
{
   const I16ErdAdderConfiguration_t *I16ErdAdderConfig;
} GridOffsetAdderErdConfiguration_t;

typedef struct
{
   struct
   {
      I16ErdAdder_t I16ErdAdder;
   } _private;
} GridOffsetAdder_t;

void GridOffsetAdder_Init(
   GridOffsetAdder_t *instance,
   I_DataModel_t *dataModel,
   const GridOffsetAdderErdConfiguration_t *config);

#endif

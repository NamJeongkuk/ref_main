/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_TripleEvap.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"

void Grid_TripleEvap(void *context)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   uint8_t someData;

   DataModel_Read(dataModel, Erd_GridLoopTripleEvapRun_Test, &someData);
   someData += 1;
   DataModel_Write(dataModel, Erd_GridLoopTripleEvapRun_Test, &someData);
}

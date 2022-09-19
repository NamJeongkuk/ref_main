/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_SingleEvap.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"

void Grid_SingleEvap(void *context)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   uint8_t someData;

   DataModel_Read(dataModel, Erd_GridLoopSingleEvapRun_Test, &someData);
   someData += 1;
   DataModel_Write(dataModel, Erd_GridLoopSingleEvapRun_Test, &someData);
}

/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_MultiDoor.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"

void Grid_MultiDoor(void *context)
{
   REINTERPRET(dataModel, context, I_DataModel_t *);
   uint8_t someData;

   DataModel_Read(dataModel, Erd_GridLoopMultiDoorRun_Test, &someData);
   someData += 1;
   DataModel_Write(dataModel, Erd_GridLoopMultiDoorRun_Test, &someData);
}

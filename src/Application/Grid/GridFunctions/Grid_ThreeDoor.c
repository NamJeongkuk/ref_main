/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_ThreeDoor.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"

void Grid_ThreeDoor(void *context)
{
   REINTERPRET(instance, context, Grid_t *);
   uint8_t someData;

   DataModel_Read(instance->_private.dataModel, Erd_GridLoopThreeDoorRun_Test, &someData);
   someData += 1;
   DataModel_Write(instance->_private.dataModel, Erd_GridLoopThreeDoorRun_Test, &someData);
}

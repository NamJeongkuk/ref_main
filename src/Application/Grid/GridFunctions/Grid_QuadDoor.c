/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_QuadDoor.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"

void Grid_QuadDoor(void *context)
{
   REINTERPRET(instance, context, Grid_t *);
   uint8_t someData;

   DataModel_Read(instance->_private.dataModel, Erd_GridLoopQuadDoorRun_Test, &someData);
   someData += 1;
   DataModel_Write(instance->_private.dataModel, Erd_GridLoopQuadDoorRun_Test, &someData);
}

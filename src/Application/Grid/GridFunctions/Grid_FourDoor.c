/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Grid_FourDoor.h"
#include "SystemErds.h"
#include "utils.h"
#include "Grid.h"

void Grid_FourDoor(void *context)
{
   REINTERPRET(instance, context, Grid_t *);
   uint8_t someData;

   DataModel_Read(instance->_private.dataModel, Erd_GridLoopFourDoorRun_Test, &someData);
   someData += 1;
   DataModel_Write(instance->_private.dataModel, Erd_GridLoopFourDoorRun_Test, &someData);
}

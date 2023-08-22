/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LOADOFFDOOROPENDATA_H
#define LOADOFFDOOROPENDATA_H

#include <stdint.h>
#include "DoorIndex.h"

typedef struct
{
   uint8_t bitmap[1];
} LoadOffDoorOpenDoorsBitmap_t;

typedef struct
{
   bool enable;
   LoadOffDoorOpenDoorsBitmap_t doorsBitmap;
   uint8_t timeoutInSeconds;
   uint8_t restartDelayInSeconds;
} LoadOffDoorOpenCompartmentData_t;

typedef struct
{
   const LoadOffDoorOpenCompartmentData_t freezerCompartmentData;
} LoadOffDoorOpenData_t;

#endif

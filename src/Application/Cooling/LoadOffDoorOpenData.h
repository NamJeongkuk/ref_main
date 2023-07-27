/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LOADOFFDOOROPENDATA_H
#define LOADOFFDOOROPENDATA_H

#include <stdint.h>

typedef struct
{
   bool enable;
   uint8_t timeoutInSeconds;
   uint8_t restartDelayInSeconds;
} LoadOffDoorOpenCompartmentData_t;

typedef struct
{
   const LoadOffDoorOpenCompartmentData_t freshFoodCompartmentData;
   const LoadOffDoorOpenCompartmentData_t freezerCompartmentData;
} LoadOffDoorOpenData_t;

#endif

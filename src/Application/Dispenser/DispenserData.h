/*!
 * @file
 * @brief dispenser parametric data struct
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSERDATA_H
#define DISPENSERDATA_H

#include "DoorInhibitDispenseTable.h"
#include <stdint.h>

typedef struct
{
   uint16_t maximumDispensedOuncesx100;
   uint16_t maximumDispenseTimeInSeconds;
   const DoorInhibitDispenseTable_t *doorInhibitWaterDispenseTable;
   const DoorInhibitDispenseTable_t *doorInhibitIceDispenseTable;
} DispenserData_t;

#endif

/*!
 * @file
 * @brief dispenser parametric data struct
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSERDATA_H
#define DISPENSERDATA_H

#include <stdint.h>

typedef struct
{
   uint16_t maximumDispensedOuncesx100;
   uint16_t maximumDispenseTimeInSeconds;
} DispenserData_t;

#endif

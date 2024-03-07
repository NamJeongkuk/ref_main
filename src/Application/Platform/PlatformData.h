/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PLATFORMDATA_H
#define PLATFORMDATA_H

#include <stdint.h>

enum
{
   Compartment_FreshFood,
   Compartment_Freezer,
   Compartment_Convertible
};
typedef uint8_t Compartment_t;

typedef struct
{
   uint8_t bitmap[1];
} CompartmentBitmap_t;

typedef struct
{
   CompartmentBitmap_t compartmentBitmap;
   uint8_t numberOfEvaporators;
} PlatformData_t;

#endif

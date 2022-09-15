/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BSPPARAMETRICCONFIGURATION_H
#define BSPPARAMETRICCONFIGURATION_H

#include "ConstArrayMap_LinearSearch.h"
#include "ConstArrayMap_BinarySearch.h"

typedef struct
{
   const ConstArrayMap_BinarySearchConfiguration_t *bspToApplicationMapConfiguration;
   const ConstArrayMap_LinearSearchConfiguration_t *applicationToBspMapConfiguration;
} BspParametricConfiguration_t;

#endif

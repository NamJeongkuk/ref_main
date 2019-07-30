/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENDIANNESSSWAPPEDDATASOURCE_H
#define ENDIANNESSSWAPPEDDATASOURCE_H

#include "DataSource_EndiannessSwapped.h"
#include "ConstMultiMap_BinarySearch.h"

typedef struct
{
   struct
   {
      DataSource_EndiannessSwapped_t dataSource;
      ConstMultiMap_BinarySearch_t multiMap;
   } _private;
} EndiannessSwappedDataSource_t;

/*!
 * @param instance
 * @param toSwap
 */
void EndiannessSwappedDataSource_Init(
  EndiannessSwappedDataSource_t *instance,
  I_DataSource_t *toSwap);

/*!
 * @param instance
 * @return
 */
I_DataSource_t * EndiannessSwappedDataSource_DataSource(
  EndiannessSwappedDataSource_t *instance);

#endif

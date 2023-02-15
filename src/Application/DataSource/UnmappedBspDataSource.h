/*!
 * @file
 * @brief The unmapped Bsp DataSource complies all ERDS in system ERDS with the storage type MappedBsp
 * into a DataSource_Zero and respond to the unused mapped ERDS and return zero if read
 * and throw away the data if its written to.
 *
 * This Module must be initalized after BspDataSource to ensure all the Mapped Erds are getting handled
 * by the Bsp.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef UNMAPPEDBSPDATASOURCE_H
#define UNMAPPEDBSPDATASOURCE_H

#include "DataSource_Zeros.h"
#include "ConstArrayMap_BinarySearch.h"

typedef struct
{
   struct
   {
      DataSource_Zeros_t dataSourceZeros;
      ConstArrayMap_BinarySearch_t erdMap;
   } _private;
} UnmappedBspDataSource_t;

/*!
 * Initialize Unmapped BSP data source components
 * @param instance
 * @param timerModule
 */
void UnmappedBspDataSource_Init(
   UnmappedBspDataSource_t *instance);

/*!
 * Get the interface to the Unmapped BSP data source
 * @param instance
 * @return
 */
I_DataSource_t *UnmappedBspDataSource_DataSource(UnmappedBspDataSource_t *instance);

#endif

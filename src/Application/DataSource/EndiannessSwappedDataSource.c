/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EndiannessSwappedDataSource.h"
#include "SystemErds.h"

static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] =
   {
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features),
      SWAPPED_ERD(Erd_BuildNumber, sizeof(uint32_t)),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, maximumLatency),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, averageLatency),
   };

static const ConstMultiMap_BinarySearchConfiguration_t multiMapConfiguration =
   {
      swappedFields,
      NUM_ELEMENTS(swappedFields),
      ELEMENT_SIZE(swappedFields),
      MEMBER_SIZE(DataSource_EndiannessSwappedSwappedField_t, erd),
      OFFSET_OF(DataSource_EndiannessSwappedSwappedField_t, erd),
      sizeof(DataSource_EndiannessSwappedSwappedField_t),
      0,
      false
   };


void EndiannessSwappedDataSource_Init(
   EndiannessSwappedDataSource_t *instance,
   I_DataSource_t *toSwap)
{
   ConstMultiMap_BinarySearch_Init(&instance->_private.multiMap, &multiMapConfiguration);

   DataSource_EndiannessSwapped_Init(
      &instance->_private.dataSource,
      toSwap,
      &instance->_private.multiMap.interface);
}

I_DataSource_t * EndiannessSwappedDataSource_DataSource(
   EndiannessSwappedDataSource_t *instance)
{
   return &instance->_private.dataSource.interface;
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATIONDATAMODEL_H
#define APPLICATIONDATAMODEL_H

#include "SystemErds.h"
#include "DataSource_Ram.h"
#include "ConstArrayMap_BinarySearch.h"
#include "ConstArrayMap_DirectIndex.h"
#include "DataSource_EndiannessSwapped.h"
#include "XMacroUtils.h"
#include "DataSource_Mapped.h"
#include "ConstBidirectionalMap_ConstArrayMap.h"
#include "ConstMultiMap_BinarySearch.h"
#include "DataSource_ApplianceApiRevision1.h"
#include "DataSource_Composite.h"
#include "ImageHeader.h"
#include "DataModel_DataSource.h"
#include "NonVolatileDataSource.h"

// Generate a structure to be used in calculating the total size of the RAM buffer required
#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Type, Swap, Io, O) \
   uint8_t CONCAT(erd, Name)[sizeof(DataType)];

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
   } Application_DataSourceRam_t;

#define EXPAND_AS_OUTPUT_COUNT_STRUCT(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_, O)(uint8_t Name ;)

#define EXPAND_AS_INPUT_COUNT_STRUCT(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_, Io)(uint8_t Name ;)

typedef struct { ERD_TABLE(EXPAND_AS_OUTPUT_COUNT_STRUCT) } OutputCountCalculator_t;
typedef struct { ERD_TABLE(EXPAND_AS_INPUT_COUNT_STRUCT) } InputCountCalculator_t;

typedef struct
{
   struct
   {
      DataModel_DataSource_t dataModel;
      DataSource_Ram_t ramDataSource;
      uint8_t erdRam[sizeof(Application_DataSourceRam_t)];
      ConstArrayMap_BinarySearch_t outputErdMap;
      ConstArrayMap_BinarySearch_t inputAndInputOutputErdMap;
      ConstArrayMap_DirectIndex_t erdConfigurationData;
      DataModel_DataSourceOutputForSingleton_t outputs[sizeof(OutputCountCalculator_t)];
      DataModel_DataSourceInputOutputForSingleton_t inputOutputs[sizeof(InputCountCalculator_t)];
      ConstArrayMap_DirectIndex_t internalToPublicDirectIndexMap;
      ConstArrayMap_BinarySearch_t publicToInternalBinarySearchMap;
      ConstBidirectionalMap_ConstArrayMap_t bidirectionalMap;
      DataSource_EndiannessSwapped_t dataSourceEndiannessSwapped;
      DataSource_Mapped_t dataSourceMapped;
      DataSource_ApplianceApiRevision1_t dataSourceApplianceApi;
      DataSource_Composite_t dataSourceComposite;
      DataSource_CompositeComponent_t ramComponent;
      DataSource_CompositeComponent_t applianceApiComponent;
      DataSource_CompositeComponent_t nvComponent;
      ConstMultiMap_BinarySearch_t multiMap;
      NonVolatileDataSource_t nvDataSource;
   } _private;
} ApplicationDataModel_t;

/*!
 * Glue code initialization internal/external data model and sources
 * @pre instance != NULL
 * @pre timerModule != NULL
 * @pre systemResetAction != NULL
 * @pre crc16Calculator != NULL
 * @param instance The data source instance
 * @param timerModule
 * @param systemResetAction
 * @param watchdog
 * @param crc16Calculator
 * @param applicationImageHeader
 * @param bootLoaderImageHeader
 */
void ApplicationDataModel_Init(
   ApplicationDataModel_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *systemResetAction,
   I_Action_t *watchdog,
   I_Crc16Calculator_t *crc16Calculator,
   I_FlashBlockGroup_t *flashBlockGroup,
   const ImageHeader_t *applicationImageHeader,
   const ImageHeader_t *bootLoaderImageHeader);

/*!
 * The data model used internal to the application. Data will read and write as is in little-endian (LSB-MSB).
 * And provides input/output data sources for application layer
 * @param instance The application DataModel instance
 * @return The data model interface
 */
I_DataModel_t * ApplicationDataModel_GetInternalDataModel(ApplicationDataModel_t *instance);

/*!
 * The data source used external to the application via GEA. Data will read and write in big-endian (MSB-LSB).
 * @param instance The application DataModel instance
 * @return The data source interface
 */
I_DataSource_t * ApplicationDataModel_GetExternalDataSource(ApplicationDataModel_t *instance);

#endif

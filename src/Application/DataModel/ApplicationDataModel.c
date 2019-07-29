/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplicationDataModel.h"
#include "utils.h"
#include "uassert.h"
#include "DataModelErdPointerAccess.h"

enum
{
   EnableSafeEvents = true,
   EnableSingletonDataModel = true
};

static const DataSource_EndiannessSwappedSwappedField_t swappedFields[] =
   {
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision),
      SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features),
      SWAPPED_ERD(Erd_BuildNumber, sizeof(uint32_t)),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, maximumLatency),
      SWAPPED_FIELD(Erd_TimerModuleDiagnosticsResult, TimerModuleDiagnosticsResults_t, averageLatency),
   };

#define EXPAND_AS_CONFIGURATION(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_RAM_, Type)({ Name COMMA OFFSET_OF(Application_DataSourceRam_t, CONCAT(erd, Name)) } COMMA)
static const DataSource_RamErdConfigurationElement_t erdConfigurations[] = { ERD_TABLE(EXPAND_AS_CONFIGURATION) };

static const ConstArrayMap_DirectIndexConfiguration_t erdConfigurationList =
   {
      erdConfigurations,
      ELEMENT_SIZE(erdConfigurations),
      NUM_ELEMENTS(erdConfigurations),
      RamErdBaseId
   };

static void InitializeRamDataSource(ApplicationDataModel_t *instance)
{
   ConstArrayMap_DirectIndex_Init(&instance->_private.erdConfigurationData, &erdConfigurationList);

   DataSource_Ram_Init(
      &instance->_private.ramDataSource,
      instance->_private.erdRam,
      sizeof(instance->_private.erdRam),
      &instance->_private.erdConfigurationData.interface);
}

static void InitializeNvDataSource(
   ApplicationDataModel_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *watchdog,
   I_Crc16Calculator_t *crc16Calculator,
   I_FlashBlockGroup_t *flashBlockGroup)
{
   NonVolatileDataSource_Init(
      &instance->_private.nvDataSource,
      timerModule,
      watchdog,
      crc16Calculator,
      flashBlockGroup);
}

#define EXPAND_AS_NV_PUBLIC_INPUT_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_PUBLIC_, Type)(CONCAT(INCLUDE_, Io)(Name COMMA))

#define EXPAND_AS_VIRTUAL_INPUT_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_VIRTUAL_, Type)(CONCAT(INCLUDE_, Io)(Name COMMA))

#define EXPAND_AS_NV_PRIVATE_INPUT_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_PRIVATE_, Type)(CONCAT(INCLUDE_, Io)(Name COMMA))

#define EXPAND_AS_RAM_INPUT_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_RAM_, Type)(CONCAT(INCLUDE_, Io)(Name COMMA))

#define EXPAND_AS_NV_PUBLIC_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_PUBLIC_, Type)(CONCAT(INCLUDE_, O)(Name COMMA))

#define EXPAND_AS_VIRTUAL_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_VIRTUAL_, Type)(CONCAT(INCLUDE_, O)(Name COMMA))

#define EXPAND_AS_NV_PRIVATE_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_NV_PRIVATE_, Type)(CONCAT(INCLUDE_, O)(Name COMMA))

#define EXPAND_AS_RAM_OUTPUT_ERDS(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_RAM_, Type)(CONCAT(INCLUDE_, O)(Name COMMA))

static const Erd_t inputAndInputOutputErds[] =
   {
      ERD_TABLE(EXPAND_AS_NV_PUBLIC_INPUT_OUTPUT_ERDS)
      ERD_TABLE(EXPAND_AS_VIRTUAL_INPUT_OUTPUT_ERDS)
      ERD_TABLE(EXPAND_AS_RAM_INPUT_OUTPUT_ERDS)
      ERD_TABLE(EXPAND_AS_NV_PRIVATE_INPUT_OUTPUT_ERDS)
   };

static const Erd_t outputErds[] =
   {
      ERD_TABLE(EXPAND_AS_NV_PUBLIC_OUTPUT_ERDS)
      ERD_TABLE(EXPAND_AS_VIRTUAL_OUTPUT_ERDS)
      ERD_TABLE(EXPAND_AS_RAM_OUTPUT_ERDS)
      ERD_TABLE(EXPAND_AS_NV_PRIVATE_OUTPUT_ERDS)
};

static const ConstArrayMap_BinarySearchConfiguration_t inputAndInputOutputErdList =
   {
      inputAndInputOutputErds,
      NUM_ELEMENTS(inputAndInputOutputErds),
      sizeof(Erd_t),
      sizeof(Erd_t),
      0,
      IS_SIGNED(Erd_t)
   };

static const ConstArrayMap_BinarySearchConfiguration_t outputErdList =
   {
      outputErds,
      NUM_ELEMENTS(outputErds),
      sizeof(Erd_t),
      sizeof(Erd_t),
      0,
      IS_SIGNED(Erd_t)
   };

static void InitializeDataModel(ApplicationDataModel_t *instance)
{
   ConstArrayMap_BinarySearch_Init(&instance->_private.outputErdMap, &outputErdList);
   ConstArrayMap_BinarySearch_Init(&instance->_private.inputAndInputOutputErdMap, &inputAndInputOutputErdList);

   DataModel_DataSource_Init(
      &instance->_private.dataModel,
      &instance->_private.dataSourceComposite.interface,
      &instance->_private.inputAndInputOutputErdMap.interface,
      &instance->_private.outputErdMap.interface,
      instance->_private.outputs,
      NUM_ELEMENTS(instance->_private.outputs),
      instance->_private.inputOutputs,
      NUM_ELEMENTS(instance->_private.inputOutputs),
      EnableSafeEvents,
      EnableSingletonDataModel);
}

static DataSource_ApplianceApiRevision1Configuration_t applianceApiConfiguration =
   {
      .header =
         {
            .bootLoader = NULL,
            .application = NULL,
            .parametric = NULL,
            .auxiliary = NULL
         },

      .foundation =
         {
            .erd =
               {
                  .manifest = Erd_ApplianceApiManifest,

                  .reset = Erd_Reset,

                  .supportedImageTypes = Erd_SupportedImageTypes,

                  .bootLoaderImageVersion = Erd_BootLoaderVersion,
                  .applicationImageVersion = Erd_ApplicationVersion,
                  .parametricImageVersion = Erd_ParametricVersion,
                  .auxiliaryImageVersion = Erd_AuxiliaryVersion
               },
         },

      .primary =
         {
            .supported = false,
         },

      .serviceMode =
         {
            .supported = false,
         },

      .bootLoader =
         {
            .supported = true,

            .erd =
               { Erd_ReadyToEnterBootLoader }
         },
   };

static void InitializeApplianceApiDataSource(
   ApplicationDataModel_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *systemResetAction,
   I_Crc16Calculator_t *crc16Calculator)
{
   DataSource_ApplianceApiRevision1_Init(
      &instance->_private.dataSourceApplianceApi,
      &applianceApiConfiguration,
      &instance->_private.ramDataSource.interface,
      timerModule,
      systemResetAction,
      crc16Calculator);
}

static void InitializeCompositeDataSource(ApplicationDataModel_t *instance)
{
   DataSource_Composite_Init(&instance->_private.dataSourceComposite);

   DataSource_Composite_InitComponent(&instance->_private.ramComponent, &instance->_private.ramDataSource.interface);
   DataSource_Composite_InitComponent(&instance->_private.applianceApiComponent, &instance->_private.dataSourceApplianceApi.interface);
   DataSource_Composite_InitComponent(&instance->_private.nvComponent, NonVolatileDataSource_GetDataSource(&instance->_private.nvDataSource));

   DataSource_Composite_Add(&instance->_private.dataSourceComposite, &instance->_private.ramComponent);
   DataSource_Composite_Add(&instance->_private.dataSourceComposite, &instance->_private.applianceApiComponent);
   DataSource_Composite_Add(&instance->_private.dataSourceComposite, &instance->_private.nvComponent);
}

#define EXPAND_AS_MAPPED_RAM_ERD_PAIR(Name, Number, DataType, Type, Swap, Io, O) \
   CONCAT(INCLUDE_RAM_, Type)({ Number COMMA Name } COMMA)

static const DataSource_MappedErdPair_t erdMappingForBidirectionalSearch[] =
   {
   ERD_TABLE(EXPAND_AS_MAPPED_RAM_ERD_PAIR)
   };

static const ConstArrayMap_DirectIndexConfiguration_t internalToPublicConfiguration =
   {
      erdMappingForBidirectionalSearch,
      ELEMENT_SIZE(erdMappingForBidirectionalSearch),
      NUM_ELEMENTS(erdMappingForBidirectionalSearch),
      RamErdBaseId
   };

static const ConstArrayMap_BinarySearchConfiguration_t publicToInternalConfiguration =
   {
      erdMappingForBidirectionalSearch,
      NUM_ELEMENTS(erdMappingForBidirectionalSearch),
      ELEMENT_SIZE(erdMappingForBidirectionalSearch),
      MEMBER_SIZE(DataSource_MappedErdPair_t, mappedErdId),
      OFFSET_OF(DataSource_MappedErdPair_t, mappedErdId),
      IS_SIGNED(Erd_t),
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

static void InitializeExternalDataSource(ApplicationDataModel_t *instance)
{
   ConstMultiMap_BinarySearch_Init(&instance->_private.multiMap, &multiMapConfiguration);

   DataSource_EndiannessSwapped_Init(
      &instance->_private.dataSourceEndiannessSwapped,
      &instance->_private.dataSourceComposite.interface,
      &instance->_private.multiMap.interface);

   ConstArrayMap_DirectIndex_Init(&instance->_private.internalToPublicDirectIndexMap, &internalToPublicConfiguration);
   ConstArrayMap_BinarySearch_Init(&instance->_private.publicToInternalBinarySearchMap, &publicToInternalConfiguration);

   ConstBidirectionalMap_ConstArrayMap_Init(
       &instance->_private.bidirectionalMap,
       &instance->_private.publicToInternalBinarySearchMap.interface,
       &instance->_private.internalToPublicDirectIndexMap.interface);

   DataSource_Mapped_InitWithPassThrough(
      &instance->_private.dataSourceMapped,
      &instance->_private.dataSourceEndiannessSwapped.interface,
      &instance->_private.bidirectionalMap.interface);
}

void ApplicationDataModel_Init(
   ApplicationDataModel_t *instance,
   TimerModule_t *timerModule,
   I_Action_t *systemResetAction,
   I_Action_t *watchdog,
   I_Crc16Calculator_t *crc16Calculator,
   I_FlashBlockGroup_t *flashBlockGroup,
   const ImageHeader_t *applicationImageHeader,
   const ImageHeader_t *bootLoaderImageHeader)
{
   uassert(instance);
   uassert(timerModule);
   uassert(systemResetAction);
   uassert(crc16Calculator);

   applianceApiConfiguration.header.application = applicationImageHeader;
   applianceApiConfiguration.header.bootLoader = bootLoaderImageHeader;

   InitializeRamDataSource(instance);
   InitializeApplianceApiDataSource(instance, timerModule, systemResetAction, crc16Calculator);
   InitializeNvDataSource(instance, timerModule, watchdog, crc16Calculator, flashBlockGroup);
   InitializeCompositeDataSource(instance);
   InitializeDataModel(instance);
   InitializeExternalDataSource(instance);

   DataModelErdPointerAccess_Write(
      &instance->_private.dataModel.interface,
      Erd_ExternalDataSource,
      ApplicationDataModel_GetExternalDataSource(instance));
}

I_DataModel_t * ApplicationDataModel_GetInternalDataModel(ApplicationDataModel_t *instance)
{
   return &instance->_private.dataModel.interface;
}

I_DataSource_t * ApplicationDataModel_GetExternalDataSource(ApplicationDataModel_t *instance)
{
   return &instance->_private.dataSourceMapped.interface;
}

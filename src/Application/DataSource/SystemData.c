/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemData.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"

typedef SystemData_t Instance_t;

static void AddDataSourceToComposite(
   Instance_t *instance,
   I_DataSource_t *dataSource,
   DataSource_CompositeComponent_t *component)
{
   DataSource_Composite_InitComponent(component, dataSource);
   DataSource_Composite_Add(&instance->_private.dataSource.composite, component);
}

static void InitializeInternalDataSource(
   Instance_t *instance,
   TimerModule_t *timerModule,
   I_AsyncDataSource_t *async,
   I_Crc16Calculator_t *crcCalculator,
   I_Action_t *systemActionForStartup,
   I_Action_t *resetAction)
{
   DataSource_Composite_Init(&instance->_private.dataSource.composite);

   RamDataSource_Init(
      &instance->_private.dataSource.ram);
   AddDataSourceToComposite(
      instance,
      RamDataSource_DataSource(&instance->_private.dataSource.ram),
      &instance->_private.dataSource.ramComponent);

   NonVolatileDataSource_Init(
      &instance->_private.dataSource.nv,
      timerModule,
      systemActionForStartup,
      async);
   AddDataSourceToComposite(
      instance,
      NonVolatileDataSource_DataSource(&instance->_private.dataSource.nv),
      &instance->_private.dataSource.nvComponent);

   ApplianceApiDataSource_Init(
      &instance->_private.dataSource.applianceApi,
      &instance->_private.dataSource.composite.interface,
      timerModule,
      resetAction,
      crcCalculator);
   AddDataSourceToComposite(
      instance,
      ApplianceApiDataSource_DataSource(&instance->_private.dataSource.applianceApi),
      &instance->_private.dataSource.applianceApiComponent);

   ServiceDiagnosticsDataSource_Init(
      &instance->_private.dataSource.serviceDiagnostics,
      &instance->_private.dataSource.composite.interface,
      timerModule);
   AddDataSourceToComposite(
      instance,
      ServiceDiagnosticsDataSource_DataSource(&instance->_private.dataSource.serviceDiagnostics),
      &instance->_private.dataSource.serviceDiagnosticsComponent);

   ConvertedDataSource_Init(
      &instance->_private.dataSource.converted,
      &instance->_private.dataSource.composite.interface);
   AddDataSourceToComposite(
      instance,
      ConvertedDataSource_DataSource(&instance->_private.dataSource.converted),
      &instance->_private.dataSource.convertedComponent);
}

static void InitializeExternalDataSource(Instance_t *instance)
{
   ExternalDataSource_Init(
      &instance->_private.dataSource.externalDataSource,
      &instance->_private.dataSource.internal);
}

static void InitializeDataModel(Instance_t *instance)
{
   DataModel_Init(
      &instance->_private.dataModel,
      &instance->_private.dataSource.internal);
}

void SystemData_Init(
   SystemData_t *instance,
   TimerModule_t *timerModule,
   I_AsyncDataSource_t *async,
   I_Crc16Calculator_t *crcCalculator,
   I_Action_t *systemActionForStartup,
   I_Action_t *resetAction)
{
   InitializeInternalDataSource(
      instance,
      timerModule,
      async,
      crcCalculator,
      systemActionForStartup,
      resetAction);

   InitializeExternalDataSource(instance);
   InitializeDataModel(instance);

   DataModel_WritePointer(
      DataModel_DataModel(&instance->_private.dataModel),
      Erd_ExternalDataSource,
      &instance->_private.dataSource.external);
}

I_DataSource_t *SystemData_InternalDataSource(SystemData_t *instance)
{
   return &instance->_private.dataSource.internal;
}

I_DataSource_t *SystemData_ExternalDataSource(SystemData_t *instance)
{
   return &instance->_private.dataSource.external;
}

I_DataModel_t *SystemData_DataModel(SystemData_t *instance)
{
   return DataModel_DataModel(&instance->_private.dataModel);
}

void SystemData_AddBspDataSource(
   SystemData_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   I_Interrupt_t *interrupt)
{
   BspDataSource_Init(
      &instance->_private.dataSource.bsp,
      timerModule,
      interrupt,
      dataModel);
   AddDataSourceToComposite(
      instance,
      BspDataSource_DataSource(&instance->_private.dataSource.bsp),
      &instance->_private.dataSource.bspComponent);

   // UnmappedBspDataSource Must be added to the DataSource Composite after BspDataSource
   UnmappedBspDataSource_Init(
      &instance->_private.dataSource.unmappedBsp);
   AddDataSourceToComposite(
      instance,
      UnmappedBspDataSource_DataSource(&instance->_private.dataSource.unmappedBsp),
      &instance->_private.dataSource.unmappedBspComponent);
}

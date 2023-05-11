/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceApiDataSource.h"
#include "SystemErds.h"
#include "Header.h"

static const DataSource_ApplianceApiRevision1Configuration_t applianceApiConfiguration = {
   .header = {
      .bootLoader = BootLoaderImageHeader,
      .application = ApplicationImageHeader,
      .parametric = ParametricImageHeader,
      .auxiliary = NULL,
   },

   .foundation = {
      .erd = {
         .manifest = Erd_ApplianceApiManifest,

         .reset = Erd_Reset,

         .supportedImageTypes = Erd_SupportedImageTypes,

         .bootLoaderImageVersion = Erd_BootLoaderVersion,
         .applicationImageVersion = Erd_ApplicationVersion,
         .parametricImageVersion = Erd_ParametricVersion,
         .auxiliaryImageVersion = Erd_AuxiliaryVersion,
      },
   },

   .primary = {
      .supported = true,
      .applianceType = ApplianceFeatureApiType_Refrigerator,
      .erd = {
         .modelNumber = Erd_ModelNumber,
         .serialNumber = Erd_SerialNumber,
         .applianceType = Erd_ApplianceType,
         .appliancePersonality = Erd_AppliancePersonality,
         .applianceRunTime = Erd_ServiceDiagnosticsRunTimeInMinutes,
      },
   },

   .serviceMode = {
      .supported = false,
   },

   .bootLoader = {
      .supported = true,
      .erd = {
         .readyToEnterBootLoader = Erd_ReadyToEnterBootLoader,
      },
   },

   .timeOfDay = {
      .supported = false,
   },

   .resetInformation = {
      .supported = true,
      .erd = {
         .resetCount = Erd_ResetCount,
         .resetReason = Erd_ResetReason,
         .secondsSinceLastReset = Erd_SecondsSinceLastReset,
         .programCounterAddressOfLastUassert = Erd_ProgramCounterAddressAtLastUassert,
      },
   },
};

void ApplianceApiDataSource_Init(
   ApplianceApiDataSource_t *instance,
   I_DataSource_t *dataSource,
   TimerModule_t *timerModule,
   I_Action_t *resetAction,
   I_Crc16Calculator_t *crcCalculator)
{
   DataSource_ApplianceApiRevision1_Init(
      &instance->_private.dataSource,
      &applianceApiConfiguration,
      dataSource,
      timerModule,
      resetAction,
      crcCalculator);
}

I_DataSource_t *ApplianceApiDataSource_DataSource(ApplianceApiDataSource_t *instance)
{
   return &instance->_private.dataSource.interface;
}

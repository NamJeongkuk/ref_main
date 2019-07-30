/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceApiDataSource.h"
#include "SystemErds.h"

#ifdef TDD_BUILD
static const ImageHeader_t bootLoaderHeader;
static const ImageHeader_t applicationHeader;
static const ImageHeader_t parametricHeader;
#define  BootLoaderImageHeader &bootLoaderHeader
#define ApplicationImageHeader &applicationHeader
#define ParametricImageHeader &parametricHeader
#else
#include "Header.h"
#endif

static DataSource_ApplianceApiRevision1Configuration_t applianceApiConfiguration =
   {
      .header =
         {
            .bootLoader = BootLoaderImageHeader,
            .application = ApplicationImageHeader,
            .parametric = ParametricImageHeader,
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
            .supported = false,

            .erd = { Erd_ReadyToEnterBootLoader }
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

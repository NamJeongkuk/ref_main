/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceApiPlugin.h"
#include "SystemErds.h"
#include "TinyTimer.h"
#include "TinyEventSubscription.h"
#include "TinyBootLoaderParametric.h"
#include "Reset.h"
#include "MemoryMap.h"
#include "utils.h"

static I_TinyDataSource_t *dataSource;
static TinyEventSubscription_t dataChangedSubscription;
static TinyTimer_t resetTimer;

static void ResetTimerExpired(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   IGNORE(timerModule);
   Reset();
}

static void DataSourceChanged(void *context, const void *_args)
{
   REINTERPRET(args, _args, const TinyDataSourceOnDataChangeArgs_t *);
   IGNORE(context);

   if(args->erd == Erd_Reset)
   {
      TinyTimerModule_t *timerModule;
      TinyDataSource_Read(dataSource, Erd_TimerModule, &timerModule);
      TinyTimerModule_Start(
         timerModule,
         &resetTimer,
         *((uint8_t *)args->data) * (TinyTimerTicks_t)1000,
         ResetTimerExpired,
         NULL);
   }
}

void ApplianceApiPlugin_Init(I_TinyDataSource_t *_dataSource)
{
   dataSource = _dataSource;

   ReadyToEnterBootLoaderState_t readyToEnterBootLoader = ReadyToEnterBootLoaderState_Ready;
   TinyDataSource_Write(dataSource, Erd_ReadyToEnterBootLoader, &readyToEnterBootLoader);

   uint8_t supportedImageTypes = (1 << ImageType_BootLoader) | (1 << ImageType_Application);
   TinyDataSource_Write(dataSource, Erd_SupportedImageTypes, &supportedImageTypes);

   const ImageHeader_t *header = BootLoaderImageHeader;
   TinyDataSource_Write(dataSource, Erd_BootLoaderVersion, &header->criticalMajorVersion);
   header = ApplicationImageHeader;
   TinyDataSource_Write(dataSource, Erd_ApplicationVersion, &header->criticalMajorVersion);

   ApplianceApiManifest_t applianceApiManifest = {
      1,
      ApplianceApi_Version1_BootLoader
   };
   TinyDataSource_Write(dataSource, Erd_ApplianceApiManifest, &applianceApiManifest);

   TinyEventSubscription_Init(&dataChangedSubscription, NULL, DataSourceChanged);
   TinyEvent_Subscribe(TinyDataSource_GetOnChangeEvent(dataSource), &dataChangedSubscription);
}

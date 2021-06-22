/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceApiPlugin.h"
#include "SystemErds.h"
#include "Timer.h"
#include "EventSubscription.h"
#include "BootLoaderParametric.h"
#include "Reset.h"
#include "MemoryMap.h"
#include "utils.h"

static I_DataSource_t *dataSource;
static EventSubscription_t dataChangedSubscription;
static Timer_t resetTimer;

static void ResetTimerExpired(void *context)
{
   IGNORE(context);
   Reset();
}

static void DataSourceChanged(void *context, const void *_args)
{
   REINTERPRET(args, _args, const DataSourceOnDataChangeArgs_t *);
   IGNORE(context);

   if(args->erd == Erd_Reset)
   {
      TimerModule_t *timerModule;
      DataSource_Read(dataSource, Erd_TimerModule, &timerModule);
      TimerModule_StartOneShot(
         timerModule,
         &resetTimer,
         *((uint8_t *)args->data) * (TimerTicks_t)1000,
         ResetTimerExpired,
         NULL);
   }
}

void ApplianceApiPlugin_Init(I_DataSource_t *_dataSource)
{
   dataSource = _dataSource;

   ReadyToEnterBootLoaderState_t readyToEnterBootLoader = ReadyToEnterBootLoaderState_Ready;
   DataSource_Write(dataSource, Erd_ReadyToEnterBootLoader, &readyToEnterBootLoader);

   uint8_t supportedImageTypes = (1 << ImageType_BootLoader) | (1 << ImageType_Application);
   DataSource_Write(dataSource, Erd_SupportedImageTypes, &supportedImageTypes);

   const ImageHeader_t *header = BootLoaderImageHeader;
   DataSource_Write(dataSource, Erd_BootLoaderVersion, &header->criticalMajorVersion);
   header = ApplicationImageHeader;
   DataSource_Write(dataSource, Erd_ApplicationVersion, &header->criticalMajorVersion);

   ApplianceApiManifest_t applianceApiManifest = {
      1,
      ApplianceApi_Version1_BootLoader
   };
   DataSource_Write(dataSource, Erd_ApplianceApiManifest, &applianceApiManifest);

   EventSubscription_Init(&dataChangedSubscription, NULL, DataSourceChanged);
   Event_Subscribe(dataSource->OnDataChange, &dataChangedSubscription);
}

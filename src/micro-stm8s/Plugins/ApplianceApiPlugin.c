/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ApplianceApiPlugin.h"
#include "MicroSystemErds.h"
#include "TinyBootLoaderParametric.h"
#include "Reset.h"
#include "MemoryMap.h"
#include "utils.h"

enum {
   ModelParametricItem = 1,
   SerialParametricItem = 2,
   Personality = 0
};

static void ResetTimerExpired(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   IGNORE(timerModule);
   Reset();
}

static void DataSourceChanged(void *context, const void *_args)
{
   REINTERPRET(instance, context, ApplianceApiPlugin_t *);
   REINTERPRET(args, _args, const TinyDataSourceOnDataChangeArgs_t *);

   if(args->erd == Erd_Reset)
   {
      TinyTimerModule_t *timerModule;
      TinyDataSource_Read(instance->_private.dataSource, Erd_TimerModule, &timerModule);
      TinyTimerModule_Start(timerModule, &instance->_private.resetTimer, *((uint8_t *)args->data), ResetTimerExpired, NULL);
   }
}

void ApplianceApiPlugin_Init(
   ApplianceApiPlugin_t *instance,
   I_TinyDataSource_t *dataSource)
{
   instance->_private.dataSource = dataSource;

   uint8_t length;
   ModelNumber_t modelSerial;
   TinyBootLoaderParametric_ReadItem(BootLoaderHeaderAddress, ModelParametricItem, &modelSerial, &length);
   TinyDataSource_Write(dataSource, Erd_ModelNumber, &modelSerial);
   TinyBootLoaderParametric_ReadItem(BootLoaderHeaderAddress, SerialParametricItem, &modelSerial, &length);
   TinyDataSource_Write(dataSource, Erd_SerialNumber, &modelSerial);

   ApplianceType_t applianceType = ApplianceType_Dishwasher;
   TinyDataSource_Write(dataSource, Erd_ApplianceType, &applianceType);

   bool readyToEnterBootLoader = true;
   TinyDataSource_Write(dataSource, Erd_ReadyToEnterBootLoader, &readyToEnterBootLoader);

   AppliancePersonality_t personality = 0;
   TinyDataSource_Write(dataSource, Erd_Personality, &personality);

   uint8_t supportedImageTypes = (1 << ImageType_BootLoader) | (1 << ImageType_Application);
   TinyDataSource_Write(dataSource, Erd_SupportedImageTypes, &supportedImageTypes);

   const ImageHeader_t *header = BootLoaderHeaderAddress;
   TinyDataSource_Write(dataSource, Erd_BootLoaderVersion, &header->criticalMajorVersion);
   header = ApplicationHeaderAddress;
   TinyDataSource_Write(dataSource, Erd_ApplicationVersion, &header->criticalMajorVersion);

   ApplianceApiManifest_t applianceApiManifest = {
      1,
      ApplianceApi_Version1_Primary | ApplianceApi_Version1_BootLoader
   };
   TinyDataSource_Write(dataSource, Erd_ApplianceApiManifest, &applianceApiManifest);

   TinyEventSubscription_Init(&instance->_private.dataChangedSubscription, instance, DataSourceChanged);
   TinyEvent_Subscribe(TinyDataSource_GetOnChangeEvent(dataSource), &instance->_private.dataChangedSubscription);
}

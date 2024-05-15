/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AndroidSignOfLifeMonitor.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "utils.h"

static const ErdWatchdogWatchedErd_t androidSignOfLifeToMonitor[] = {
   {
      .watchedErd = Erd_PrimaryUiSignOfLife,
      .faultErd = Erd_AndroidSignOfLifeDebugFault,
      .timeoutSeconds = 5,
   }
};

static const ErdWatchdogConfiguration_t config = {
   .watchedErds = androidSignOfLifeToMonitor,
   .watchedErdCount = NUM_ELEMENTS(androidSignOfLifeToMonitor)
};

static void InitializeAndroidWatchdog(AndroidSignOfLifeMonitor_t *instance)
{
   ErdWatchdog_Init(
      &instance->_private.androidWatchdog,
      &config,
      DataModel_AsDataSource(instance->_private.dataModel),
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule));
}

static void AndroidBoardInSystemChanged(void *context, const void *_args)
{
   AndroidSignOfLifeMonitor_t *instance = context;
   const bool *androidIsInSystem = _args;

   if(*androidIsInSystem)
   {
      DataModel_Unsubscribe(
         instance->_private.dataModel,
         Erd_AndroidSbcBoardInSystem,
         &instance->_private.androidInSystemChanged);
      InitializeAndroidWatchdog(instance);
   }
}

static bool AndroidSbcBoardIsInSystem(AndroidSignOfLifeMonitor_t *instance)
{
   bool androidIsInSystem;
   DataModel_Read(
      instance->_private.dataModel,
      Erd_AndroidSbcBoardInSystem,
      &androidIsInSystem);

   return androidIsInSystem;
}

void AndroidSignOfLifeMonitor_Init(
   AndroidSignOfLifeMonitor_t *instance,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;

   if(AndroidSbcBoardIsInSystem(instance))
   {
      InitializeAndroidWatchdog(instance);
   }
   else
   {
      EventSubscription_Init(
         &instance->_private.androidInSystemChanged,
         instance,
         AndroidBoardInSystemChanged);
      DataModel_Subscribe(
         dataModel,
         Erd_AndroidSbcBoardInSystem,
         &instance->_private.androidInSystemChanged);
   }
}

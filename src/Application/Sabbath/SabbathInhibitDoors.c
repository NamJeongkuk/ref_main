/*!
 * @file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SabbathInhibitDoors.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

enum
{
   Closed = false
};

enum
{
   SabbathGpioDelayInMilliseconds = 100 // Electron travel time
};

static void ClearOverrideRequests(void *context)
{
   SabbathInhibitDoors_t *instance = context;

   for(uint8_t index = 0; index < instance->_private.config->numberOfPairs; index++)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->doorOverrideErdPairList[index].doorStatusOverrideRequestErd,
         clear);
   }
}

static void InhibitDoorsInSabbathMode(void *context)
{
   SabbathInhibitDoors_t *instance = context;

   bool sabbathState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sabbathModeErd,
      &sabbathState);

   bool enhancedSabbathState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enhancedSabbathModeErd,
      &enhancedSabbathState);

   if(sabbathState || enhancedSabbathState)
   {
      for(uint8_t index = 0; index < instance->_private.config->numberOfPairs; index++)
      {
         bool closed = Closed;
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->doorOverrideErdPairList[index].doorStatusOverrideValueErd,
            &closed);

         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->doorOverrideErdPairList[index].doorStatusOverrideRequestErd,
            set);
      }

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->sabbathGpioErd,
         clear);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->sabbathGpioErd,
         set);

      // Delay to allow for electron travel time to the new ground connection
      TimerModule_StartOneShot(
         DataModelErdPointerAccess_GetTimerModule(
            instance->_private.dataModel,
            Erd_TimerModule),
         &instance->_private.timer,
         SabbathGpioDelayInMilliseconds,
         ClearOverrideRequests,
         instance);
   }
};

static bool SabbathDoorLogicNeedsChecked(void *context, const void *args)
{
   SabbathInhibitDoors_t *instance = context;
   const DataSourceOnDataChangeArgs_t *onChangeArgs = args;

   if(onChangeArgs->erd == instance->_private.config->enhancedSabbathModeErd ||
      onChangeArgs->erd == instance->_private.config->sabbathModeErd)
   {
      return true;
   }

   return false;
}

static void OnDataModelChange(void *context, const void *args)
{
   if(SabbathDoorLogicNeedsChecked(context, args))
   {
      InhibitDoorsInSabbathMode(context);
   }
}

void SabbathInhibitDoors_Init(
   SabbathInhibitDoors_t *instance,
   I_DataModel_t *dataModel,
   const SabbathInhibitDoorsConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   InhibitDoorsInSabbathMode(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   Event_Subscribe(
      instance->_private.dataModel->OnDataChange,
      &instance->_private.onDataModelChangeSubscription);
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterStateResolver.h"
#include "utils.h"
#include "WaterFilterState.h"
#include "WaterFilterLifeStatus.h"

static bool DemoModeEnabled(WaterFilterStateResolver_t *instance)
{
   bool enableDemoModeStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->enableDemoModeStatusErd,
      &enableDemoModeStatus);

   return enableDemoModeStatus;
}

static bool LeakDetected(WaterFilterStateResolver_t *instance)
{
   bool leakDetected;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->leakDetectedErd,
      &leakDetected);

   return leakDetected;
}

static bool FilterError(WaterFilterStateResolver_t *instance)
{
   bool filterError;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->filterErrorErd,
      &filterError);

   return filterError;
}

static bool BypassPlugInstalled(WaterFilterStateResolver_t *instance)
{
   bool bypassPlugInstalled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->bypassPlugInstalledErd,
      &bypassPlugInstalled);

   return bypassPlugInstalled;
}

static WaterFilterLifeStatus_t WaterFilterLifeStatus(WaterFilterStateResolver_t *instance)
{
   WaterFilterLifeStatus_t waterFilterLifeStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterLifeStatusErd,
      &waterFilterLifeStatus);

   return waterFilterLifeStatus;
}

static void WriteWaterFilterStateTo(WaterFilterStateResolver_t *instance, WaterFilterState_t state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->resolvedFilterStateErd,
      &state);
}

static void ResolveWaterFilterState(WaterFilterStateResolver_t *instance)
{
   WaterFilterLifeStatus_t waterFilterLifeStatus = WaterFilterLifeStatus(instance);
   WaterFilterState_t waterFilterState = WaterFilterState_Good;

   if(DemoModeEnabled(instance))
   {
      waterFilterState = WaterFilterState_Good;
   }
   else if(LeakDetected(instance))
   {
      waterFilterState = WaterFilterState_Leak;
   }
   else if(FilterError(instance))
   {
      waterFilterState = WaterFilterState_Error;
   }
   else if(BypassPlugInstalled(instance))
   {
      waterFilterState = WaterFilterState_Bypass;
   }
   else if(waterFilterLifeStatus == WaterFilterLifeStatus_Expired)
   {
      waterFilterState = WaterFilterState_Expired;
   }
   else if(waterFilterLifeStatus == WaterFilterLifeStatus_Replace)
   {
      waterFilterState = WaterFilterState_Replace;
   }

   WriteWaterFilterStateTo(instance, waterFilterState);
}

static bool ErdIsAWaterFilterStateErd(WaterFilterStateResolver_t *instance, Erd_t erd)
{
   return (erd == instance->_private.config->enableDemoModeStatusErd ||
      erd == instance->_private.config->leakDetectedErd ||
      erd == instance->_private.config->filterErrorErd ||
      erd == instance->_private.config->bypassPlugInstalledErd ||
      erd == instance->_private.config->waterFilterLifeStatusErd);
}

static void OnDataModelChange(void *context, const void *_args)
{
   WaterFilterStateResolver_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(ErdIsAWaterFilterStateErd(instance, args->erd))
   {
      ResolveWaterFilterState(instance);
   }
}

void WaterFilterStateResolver_Init(
   WaterFilterStateResolver_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterStateResolverConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   ResolveWaterFilterState(instance);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}

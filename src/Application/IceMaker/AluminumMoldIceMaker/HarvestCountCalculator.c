/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "HarvestCountCalculator.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"

static TemperatureDegFx100_t IceMakerTemperature(HarvestCountCalculator_t *instance)
{
   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakerFilteredTemperatureInDegFx100Erd,
      &temperature);
   return temperature;
}

static void MinimumFreezeTimerExpired(void *context)
{
   HarvestCountCalculator_t *instance = context;
   instance->_private.minimumFreezeTimerIsExpired = true;
}

static void StartMinimumFreezeTimer(HarvestCountCalculator_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.minimumFreezeTimer,
      instance->_private.aluminumMoldIceMakerData->minimumFreezeTimeInMinutes * MSEC_PER_MIN,
      MinimumFreezeTimerExpired,
      instance);
}

static void StopMinimumFreezeTimer(HarvestCountCalculator_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.minimumFreezeTimer);
}

static void StopHarvestCountTimer(HarvestCountCalculator_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.harvestCountTimer);
}

static void CalculateHarvestCount(void *context)
{
   HarvestCountCalculator_t *instance = context;

   TemperatureDegFx100_t iceMakerTemperature = IceMakerTemperature(instance);
   if(iceMakerTemperature <= instance->_private.aluminumMoldIceMakerData->startIntegrationTemperatureInDegFx100)
   {
      if(!TimerModule_IsRunning(DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule), &instance->_private.minimumFreezeTimer) &&
         !instance->_private.minimumFreezeTimerIsExpired)
      {
         StartMinimumFreezeTimer(instance);
      }

      instance->_private.integrationTemperatureCount +=
         (instance->_private.aluminumMoldIceMakerData->startIntegrationTemperatureInDegFx100 - iceMakerTemperature);

      if(instance->_private.minimumFreezeTimerIsExpired &&
         (instance->_private.integrationTemperatureCount >= instance->_private.aluminumMoldIceMakerData->freezeIntegrationLimitInDegFx100TimesSeconds))
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->harvestCountIsReadyToHarvestErd,
            set);
         StopHarvestCountTimer(instance);
      }
   }
   else
   {
      StopMinimumFreezeTimer(instance);
      instance->_private.integrationTemperatureCount = 0;
      instance->_private.minimumFreezeTimerIsExpired = false;
   }
}

static void StartPeriodicTimerForHarvestCount(HarvestCountCalculator_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.harvestCountTimer,
      MSEC_PER_SEC,
      CalculateHarvestCount,
      instance);
}

static void HarvestCountCalculationRequestChanged(void *context, const void *args)
{
   HarvestCountCalculator_t *instance = context;
   const bool *harvestCountCalculationRequested = args;

   if(*harvestCountCalculationRequested)
   {
      instance->_private.integrationTemperatureCount = 0;
      instance->_private.minimumFreezeTimerIsExpired = false;

      CalculateHarvestCount(instance);
      StartPeriodicTimerForHarvestCount(instance);
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->harvestCountIsReadyToHarvestErd,
         clear);
      StopMinimumFreezeTimer(instance);
      StopHarvestCountTimer(instance);
   }
}

void HarvestCountCalculator_Init(
   HarvestCountCalculator_t *instance,
   I_DataModel_t *dataModel,
   const HarvestCountCalculatorConfiguration_t *config,
   const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.aluminumMoldIceMakerData = aluminumMoldIceMakerData;

   bool harvestCountCalculationRequested;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->harvestCountCalculationRequestErd,
      &harvestCountCalculationRequested);
   HarvestCountCalculationRequestChanged(instance, &harvestCountCalculationRequested);

   EventSubscription_Init(
      &instance->_private.harvestCountCalculationRequestSubscription,
      instance,
      HarvestCountCalculationRequestChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->harvestCountCalculationRequestErd,
      &instance->_private.harvestCountCalculationRequestSubscription);
}

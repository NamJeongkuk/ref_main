/*!
 * @file
 * @brief When requested this module takes care of calculating the temperature integration count and the minimum freeze time.
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
      instance->_private.config->moldFilteredTemperatureInDegFx100Erd,
      &temperature);
   return temperature;
}

static void StopMinimumFreezeTimer(HarvestCountCalculator_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.minimumFreezeTimer);
}

static void IncrementMinimumFreezeTimerCounter(void *context)
{
   HarvestCountCalculator_t *instance = context;
   uint8_t currentFreezeTime;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->moldIceMakerMinimumFreezeTimeCounterInMinutesErd,
      &currentFreezeTime);

   currentFreezeTime++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldIceMakerMinimumFreezeTimeCounterInMinutesErd,
      &currentFreezeTime);

   if(currentFreezeTime >= instance->_private.aluminumMoldIceMakerData->freezeData.minimumFreezeTimeInMinutes)
   {
      instance->_private.minimumFreezeTimeIsSatisfied = true;
   }

   if(currentFreezeTime == UINT8_MAX)
   {
      StopMinimumFreezeTimer(instance);
   }
}

static void StartMinimumFreezeTimer(HarvestCountCalculator_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.minimumFreezeTimer,
      MSEC_PER_MIN,
      IncrementMinimumFreezeTimerCounter,
      instance);
}

static void ClearMinimumFreezeTimeCounter(HarvestCountCalculator_t *instance)
{
   uint8_t clearVal = 0;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldIceMakerMinimumFreezeTimeCounterInMinutesErd,
      &clearVal);
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
   uint32_t integrationCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->moldFreezeIntegrationCountErd,
      &integrationCount);

   TemperatureDegFx100_t iceMakerTemperature = IceMakerTemperature(instance);
   if(iceMakerTemperature <= instance->_private.aluminumMoldIceMakerData->freezeData.startIntegrationTemperatureInDegFx100)
   {
      if(!TimerModule_IsRunning(DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule), &instance->_private.minimumFreezeTimer) &&
         !instance->_private.minimumFreezeTimeIsSatisfied)
      {
         StartMinimumFreezeTimer(instance);
      }

      integrationCount +=
         (instance->_private.aluminumMoldIceMakerData->freezeData.startIntegrationTemperatureInDegFx100 - iceMakerTemperature);

      if(instance->_private.minimumFreezeTimeIsSatisfied &&
         (integrationCount >= instance->_private.aluminumMoldIceMakerData->freezeData.freezeIntegrationLimitInDegFx100TimesSeconds))
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->harvestCountIsReadyToHarvestErd,
            set);
         StopHarvestCountTimer(instance);
         StopMinimumFreezeTimer(instance);
      }
   }
   else
   {
      StopMinimumFreezeTimer(instance);
      ClearMinimumFreezeTimeCounter(instance);
      integrationCount = 0;
      instance->_private.minimumFreezeTimeIsSatisfied = false;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldFreezeIntegrationCountErd,
      &integrationCount);
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
      uint32_t integrationCount = 0;

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->moldFreezeIntegrationCountErd,
         &integrationCount);

      instance->_private.minimumFreezeTimeIsSatisfied = false;

      ClearMinimumFreezeTimeCounter(instance);
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

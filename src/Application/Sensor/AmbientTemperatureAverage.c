/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "AmbientTemperatureAverage.h"
#include "Filter_WindowAverage.h"
#include "SystemErds.h"
#include "Timer.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"

static void AmbientWindowAverageTemperatureFilterReadAndWriteToErd(void *context)
{
   AmbientTemperatureAverage_t *instance = context;
   TemperatureDegFx100_t crossAmbientWindowAverageTemperature;

   Filter_Read(&instance->_private.filter.interface, &crossAmbientWindowAverageTemperature);

   DataModel_Write(
      instance->_private.dataModel,
      Erd_Ambient_WindowAveragedTemperatureInDegFx100,
      &crossAmbientWindowAverageTemperature);
}

static void CallbackForFeedingFilter(void *context)
{
   AmbientTemperatureAverage_t *instance = context;
   TemperatureDegFx100_t temperature;

   DataModel_Read(
      instance->_private.dataModel,
      Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
      &temperature);

   Filter_Feed(&instance->_private.filter.interface, &temperature);

   AmbientWindowAverageTemperatureFilterReadAndWriteToErd(context);
}

static void StartPeriodicTimerForFilterFeeding(AmbientTemperatureAverage_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.sampleFeedTimer,
      AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN,
      CallbackForFeedingFilter,
      instance);
}

static void AmbientTemperatureChangedForTheFirstTime(void *context, const void *args)
{
   AmbientTemperatureAverage_t *instance = context;
   const TemperatureDegFx100_t *temperature = args;

   DataModel_Unsubscribe(
      instance->_private.dataModel,
      Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
      &instance->_private.subscription);

   Filter_WindowAverage_Init(
      &instance->_private.filter,
      instance->_private.filterI16,
      AmbientTemperatureAverageSampleCount,
      true,
      sizeof(int16_t));

   Filter_Seed(&instance->_private.filter.interface, temperature);

   AmbientWindowAverageTemperatureFilterReadAndWriteToErd(instance);
   StartPeriodicTimerForFilterFeeding(instance);
}

void AmbientTemperatureAverage_Init(AmbientTemperatureAverage_t *instance, I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;
   TemperatureDegFx100_t temperature;

   DataModel_Read(
      dataModel,
      Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
      &temperature);

   DataModel_Write(
      dataModel,
      Erd_Ambient_WindowAveragedTemperatureInDegFx100,
      &temperature);

   EventSubscription_Init(&instance->_private.subscription, instance, AmbientTemperatureChangedForTheFirstTime);
   DataModel_Subscribe(
      instance->_private.dataModel,
      Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
      &instance->_private.subscription);
}

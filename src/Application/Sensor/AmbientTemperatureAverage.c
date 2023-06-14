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

   Filter_Read(&instance->filter.interface, &crossAmbientWindowAverageTemperature);

   DataModel_Write(
      instance->dataModel,
      Erd_Ambient_WindowAveragedTemperatureInDegFx100,
      &crossAmbientWindowAverageTemperature);
}

static void CallbackForFeedingFilter(void *context)
{
   AmbientTemperatureAverage_t *instance = context;
   TemperatureDegFx100_t temperature;

   DataModel_Read(
      instance->dataModel,
      Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
      &temperature);

   Filter_Feed(&instance->filter.interface, &temperature);

   AmbientWindowAverageTemperatureFilterReadAndWriteToErd(context);
}

static void StartPeriodicTimerForFilterFeeding(AmbientTemperatureAverage_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->dataModel, Erd_TimerModule),
      &instance->sampleFeedTimer,
      AmbientTemperatureAverageSampleFrequencyInMinutes * MSEC_PER_MIN,
      CallbackForFeedingFilter,
      instance);
}

void AmbientTemperatureAverage_Init(AmbientTemperatureAverage_t *instance, I_DataModel_t *dataModel)
{
   instance->dataModel = dataModel;
   TemperatureDegFx100_t temperature;

   DataModel_Read(
      dataModel,
      Erd_Ambient_FilteredTemperatureResolvedInDegFx100,
      &temperature);

   Filter_WindowAverage_Init(
      &instance->filter,
      instance->filterI16,
      AmbientTemperatureAverageSampleCount,
      true,
      sizeof(int16_t));

   Filter_Seed(&instance->filter.interface, &temperature);

   AmbientWindowAverageTemperatureFilterReadAndWriteToErd(instance);
   StartPeriodicTimerForFilterFeeding(instance);
}

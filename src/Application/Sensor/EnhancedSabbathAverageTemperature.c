/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EnhancedSabbathAverageTemperature.h"
#include "utils.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "TemperatureDegFx100.h"

enum
{
   TimeBetweenReadsInMsec = 100
};

static void ResetFilterAndRestartPeriodicTimer(void *context);

static void CalculateAverage(void *context)
{
   EnhancedSabbathAverageTemperature_t *instance = context;

   TemperatureDegFx100_t temperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->unfilteredTemperatureInDegFx100Erd,
      &temperature);

   Filter_Feed(instance->_private.filter, &temperature);
}

static void StartPeriodicTimer(EnhancedSabbathAverageTemperature_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer,
      TimeBetweenReadsInMsec,
      CalculateAverage,
      instance);
}

static void StartRandomNumberTimer(EnhancedSabbathAverageTemperature_t *instance)
{
   TimerTicks_t randomAdditionalTimeInMsec;
   Input_Read(instance->_private.randomNumberGeneratorInput, &randomAdditionalTimeInMsec);

   TimerTicks_t randomNumberWithMinInMsec = (instance->_private.minTimeBetweenTemperatureAveragingInMinutes * MSEC_PER_MIN) + randomAdditionalTimeInMsec;

   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.randomTimer,
      randomNumberWithMinInMsec,
      ResetFilterAndRestartPeriodicTimer,
      instance);
}

static void ResetFilterAndRestartPeriodicTimer(void *context)
{
   EnhancedSabbathAverageTemperature_t *instance = context;

   TemperatureDegFx100_t filterTemperature;
   Filter_Read(instance->_private.filter, &filterTemperature);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->overrideValueErd,
      &filterTemperature);

   Filter_Reset(instance->_private.filter);
   StartRandomNumberTimer(instance);
}

static void SetEnhancedOverrideRequestTo(EnhancedSabbathAverageTemperature_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->overrideRequestErd,
      &state);
}

static void StopActiveTimers(EnhancedSabbathAverageTemperature_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.randomTimer);
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodicTimer);
}

static void OnEnhancedSabbathModeStatusChange(void *context, const void *_args)
{
   EnhancedSabbathAverageTemperature_t *instance = context;
   const bool *status = _args;

   if(*status)
   {
      TemperatureDegFx100_t temperature;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->filteredTemperatureInDegFx100Erd,
         &temperature);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->overrideValueErd,
         &temperature);
      Filter_Seed(instance->_private.filter, &temperature);

      SetEnhancedOverrideRequestTo(instance, SET);
      StartPeriodicTimer(instance);
      StartRandomNumberTimer(instance);
   }
   else
   {
      SetEnhancedOverrideRequestTo(instance, CLEAR);
      StopActiveTimers(instance);
      Filter_Reset(instance->_private.filter);
   }
}

void EnhancedSabbathAverageTemperature_Init(
   EnhancedSabbathAverageTemperature_t *instance,
   I_DataModel_t *dataModel,
   I_Filter_t *filter,
   I_Input_t *randomNumberGeneratorInput,
   uint8_t minTimeBetweenTemperatureAveragingInMinutes,
   const EnhancedSabbathAverageTemperatureConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.filter = filter;
   instance->_private.randomNumberGeneratorInput = randomNumberGeneratorInput;
   instance->_private.minTimeBetweenTemperatureAveragingInMinutes = minTimeBetweenTemperatureAveragingInMinutes;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.onEnhancedSabbathModeStatusChange,
      instance,
      OnEnhancedSabbathModeStatusChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->enhancedSabbathModeStatusErd,
      &instance->_private.onEnhancedSabbathModeStatusChange);

   bool enhancedSabbathModeStatus;
   DataModel_Read(
      dataModel,
      config->enhancedSabbathModeStatusErd,
      &enhancedSabbathModeStatus);
   OnEnhancedSabbathModeStatusChange(instance, &enhancedSabbathModeStatus);
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENHANCEDSABBATHAVERAGETEMPERATURE_H
#define ENHANCEDSABBATHAVERAGETEMPERATURE_H

#include "I_DataModel.h"
#include "I_Filter.h"
#include "Timer.h"

enum
{
   EnhancedSabbathAverageTemperature_TotalNumberOfReadsPerSample = 20
};

typedef struct
{
   Erd_t filteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t unfilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t overrideRequestErd; // bool
   Erd_t overrideValueErd; // TemperatureDegFx100_t
   Erd_t enhancedSabbathModeStatusErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} EnhancedSabbathAverageTemperatureConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t onEnhancedSabbathModeStatusChange;
      Timer_t periodicTimer;
      Timer_t randomTimer;
      I_DataModel_t *dataModel;
      const EnhancedSabbathAverageTemperatureConfig_t *config;
      I_Filter_t *filter;
      I_Input_t *randomNumberGeneratorInput;
      uint8_t minTimeBetweenTemperatureAveragingInMinutes;
   } _private;
} EnhancedSabbathAverageTemperature_t;

/*!
 * @param instance
 * @param dataModel
 * @param filter
 * @param randomNumberGeneratorInput
 * @param minTimeBetweenTemperatureAveragingInMinutes
 * @param config
 */
void EnhancedSabbathAverageTemperature_Init(
   EnhancedSabbathAverageTemperature_t *instance,
   I_DataModel_t *dataModel,
   I_Filter_t *filter,
   I_Input_t *randomNumberGeneratorInput,
   uint8_t minTimeBetweenTemperatureAveragingInMinutes,
   const EnhancedSabbathAverageTemperatureConfig_t *config);

#endif

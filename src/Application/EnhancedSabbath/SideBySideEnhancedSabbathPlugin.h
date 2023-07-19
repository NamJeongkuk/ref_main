/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEENHANCEDSABBATHPLUGIN_H
#define SIDEBYSIDEENHANCEDSABBATHPLUGIN_H

#include "I_DataModel.h"
#include "Filter_WindowAverage.h"
#include "EnhancedSabbathAverageTemperature.h"
#include "Input_EnhancedSabbathRandomAdditionalMinute.h"

typedef struct
{
   struct
   {
      Filter_WindowAverage_t freshFoodEnhancedSabbathWindowAverageFilter;
      Filter_WindowAverage_t freezerEnhancedSabbathWindowAverageFilter;
      int16_t freshFoodFilterI16[EnhancedSabbathAverageTemperature_TotalNumberOfReadsPerSample];
      int16_t freezerFilterI16[EnhancedSabbathAverageTemperature_TotalNumberOfReadsPerSample];

      Input_EnhancedSabbathRandomAdditionalMinute_t freshFoodEnhancedSabbathRandomAdditionalMinuteInput;
      Input_EnhancedSabbathRandomAdditionalMinute_t freezerEnhancedSabbathRandomAdditionalMinuteInput;

      EnhancedSabbathAverageTemperature_t freshFoodEnhancedSabbathAverageTemperature;
      EnhancedSabbathAverageTemperature_t freezerEnhancedSabbathAverageTemperature;
   } _private;
} SideBySideEnhancedSabbathPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideEnhancedSabbathPlugin_Init(
   SideBySideEnhancedSabbathPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif

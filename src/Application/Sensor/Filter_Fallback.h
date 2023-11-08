/*!
 * @file
 * @brief Fallback filter
 *
 * The filter wraps a filter and modifies the output to either be the fallback value
 * or the wrapped filter value.
 *
 * While the thermistor is valid, if there are bad reading counter max number of bad
 * counters in a row, then it will output the fallback value. Thermistor is considered invalid.
 *
 * While the thermistor is invalid, if there are good reading counter max number of
 * good counters in a row, then it will seed the wrapped filter with the input value.
 * Thermistor is considered valid again.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILTER_FALLBACK_H
#define FILTER_FALLBACK_H

#include "I_Filter.h"
#include "Event_Synchronous.h"
#include "I_DataModel.h"
#include "InputOutput_Simple.h"
#include <stdint.h>

typedef struct
{
   int16_t filteredValue;
   bool isValid;
} Filter_Fallback_Data_t;

typedef struct
{
   I_Filter_t interface;

   struct
   {
      I_Filter_t *wrappedFilter;
      I_DataModel_t *dataModel;
      int16_t invalidValueDegFx100;
      int16_t fallbackValueDegFx100;
      uint16_t goodReadingCounterMax;
      uint16_t badReadingCounterMax;
      uint16_t goodReadingCounter;
      uint16_t badReadingCounter;
      bool isValid;
      bool isReady;
      InputOutput_Simple_t readyInputOutput;
   } _private;
} Filter_Fallback_t;

/*!
 * @brief Fallback filter
 *
 * @param instance the filter
 * @param wrappedFilter the input filter
 * @param dataModel the data model
 * @param invalidValueDegFx100 invalid value in degFx100
 * @param fallbackValueDegFx100 fallback value in degFx100
 * @param goodReadingCounterMax good reading counter max
 * @param badReadingCounterMax bad reading counter max
 */
void Filter_Fallback_Init(
   Filter_Fallback_t *instance,
   I_Filter_t *wrappedFilter,
   I_DataModel_t *dataModel,
   int16_t invalidValueDegFx100,
   int16_t fallbackValueDegFx100,
   uint16_t goodReadingCounterMax,
   uint16_t badReadingCounterMax);

#endif

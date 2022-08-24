/*!
 * @file
 * @brief Scaled filter
 *
 * The filter wraps a filter and scales the input by the scaling factor
 * to the wrapped filter and then descales the outputs from the wrapped filter
 * by the scaling factor.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILTER_SCALED_H
#define FILTER_SCALED_H

#include "I_Filter.h"
#include "Event_Synchronous.h"
#include <stdint.h>

typedef struct
{
   I_Filter_t interface;

   struct
   {
      I_Filter_t *wrappedFilter;
      uint8_t scalingFactor;
   } _private;
} Filter_Scaled_t;

/*!
 * @brief Scaled filter
 *
 * @param instance the filter
 * @param wrappedFilter the input filter
 * @param scalingFactor the scaling factor
 */
void Filter_Scaled_Init(
   Filter_Scaled_t *instance,
   I_Filter_t *wrappedFilter,
   uint8_t scalingFactor);

#endif

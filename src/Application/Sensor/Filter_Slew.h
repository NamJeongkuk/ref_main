/*!
 * @file
 * @brief Slew filter
 *
 * The filter wraps a filter and slews the output by limiting how much it can change
 * (no more than the slew limit)from the previous wrapped filter value.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILTER_SLEW_H
#define FILTER_SLEW_H

#include "I_Filter.h"
#include "Event_Synchronous.h"
#include <stdint.h>

typedef struct
{
   I_Filter_t interface;

   struct
   {
      I_Filter_t *wrappedFilter;
      int16_t previousWrappedFilterValue;
      bool hasAPreviousValue;
      uint16_t slewLimitx100;
   } _private;
} Filter_Slew_t;

/*!
 * @brief Slew filter
 *
 * @param instance the filter
 * @param wrappedFilter the input filter
 * @param slewLimitx100 the slew limit x100
 */
void Filter_Slew_Init(
   Filter_Slew_t *instance,
   I_Filter_t *wrappedFilter,
   uint16_t slewLimitx100);

#endif

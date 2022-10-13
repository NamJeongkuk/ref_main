/*!
 * @file
 * @brief Long term average filter.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FILTER_LONGTERMAVERAGE_H
#define FILTER_LONGTERMAVERAGE_H

#include <stdint.h>
#include "I_Filter.h"
#include "I_Input.h"
#include "Event_Synchronous.h"

typedef struct
{
   I_Filter_t interface;

   struct
   {
      I_Input_t readyInput;
      Event_Synchronous_t onReady;

      int16_t average1;
      int16_t average2;
      int16_t average3;
      uint8_t beta;
   } _private;
} Filter_LongTermAverage_t;

/*!
 * Initialize a long term average filter.
 * @param instance The filter.
 * @param beta The beta used for the filter. Must be greater or equal to 1.
 */
void Filter_LongTermAverage_Init(
   Filter_LongTermAverage_t *instance,
   uint8_t beta);

#endif

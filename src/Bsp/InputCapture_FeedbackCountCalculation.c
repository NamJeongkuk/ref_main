/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "InputCapture_FeedbackCountCalculation.h"

enum
{
   Mtu4_MaxTgrCount = 0xFFFF,
   OverflowCounts = Mtu4_MaxTgrCount + 1
};

InputCaptureCounts_t CalculateFeedbackCounts(uint16_t previousCounts, uint16_t currentCounts, uint8_t overflows)
{
   if(currentCounts < previousCounts)
   {
      overflows--;
   }

   return (InputCaptureCounts_t)OverflowCounts * overflows + (uint16_t)(currentCounts - previousCounts);
}

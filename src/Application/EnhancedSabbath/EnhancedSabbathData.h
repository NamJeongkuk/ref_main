/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENHANCEDSABBATHDATA_H
#define ENHANCEDSABBATHDATA_H

#include <stdint.h>

typedef struct
{
   uint8_t numberOfFreshFoodDefrostsBeforeFreezerDefrost;
   uint8_t minTimeBetweenTemperatureAveragingInMinutes;
} EnhancedSabbathData_t;

#endif

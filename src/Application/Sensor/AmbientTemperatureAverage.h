/*!
 * @file
 * @brief  This file implements an ambient temperature average needed for a cross-ambient offset.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef AMBIENTTEMPERATUREAVERAGE_H
#define AMBIENTTEMPERATUREAVERAGE_H

#include "I_DataModel.h"
#include "Filter_WindowAverage.h"
#include "Timer.h"
#include <stdint.h>

enum
{
   AmbientTemperatureAverageSampleCount = 24,
   AmbientTemperatureAverageSampleFrequencyInMinutes = 5
};

typedef struct
{
   Filter_WindowAverage_t filter;
   int16_t filterI16[AmbientTemperatureAverageSampleCount];
   Timer_t sampleFeedTimer;
   I_DataModel_t *dataModel;
} AmbientTemperatureAverage_t;

void AmbientTemperatureAverage_Init(AmbientTemperatureAverage_t *instance, I_DataModel_t *dataModel);

#endif

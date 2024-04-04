/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODNOFREEZELIMITTRIPPEDFAULTDETECTOR_H
#define FRESHFOODNOFREEZELIMITTRIPPEDFAULTDETECTOR_H

#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   Erd_t calculateGridLineErd; // TwoDimensionalCalculatedGridLines_t
   Erd_t freshFoodFilteredTemperatureErd; // TemperatureDegFx100_t
   Erd_t freshFoodNoFreezeLimitTrippedFaultErd; // bool
} FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t freshFoodTemperatureSubscription;
      const FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t *config;
   } _private;
} FreshFoodNoFreezeLimitTrippedFaultDetector_t;

void FreshFoodNoFreezeLimitTrippedFaultDetector_Init(
   FreshFoodNoFreezeLimitTrippedFaultDetector_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t *config);

#endif

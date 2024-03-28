/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Filter_Scaled.h"
#include "utils.h"

static void Seed(I_Filter_t *_instance, const void *_seedValue)
{
   REINTERPRET(instance, _instance, Filter_Scaled_t *);

   const int16_t *seedValue = _seedValue;
   int32_t scaledSeedValue = (int32_t)*seedValue * instance->_private.scalingFactor;
   Filter_Seed(instance->_private.wrappedFilter, &scaledSeedValue);
}

static void Feed(I_Filter_t *_instance, const void *_newValue)
{
   REINTERPRET(instance, _instance, Filter_Scaled_t *);

   const int16_t *feedValue = _newValue;
   int32_t scaledFeedValue = (int32_t)*feedValue * instance->_private.scalingFactor;
   Filter_Feed(instance->_private.wrappedFilter, &scaledFeedValue);
}

static void Read(I_Filter_t *_instance, void *value)
{
   REINTERPRET(instance, _instance, Filter_Scaled_t *);

   int32_t filterValue;
   Filter_Read(instance->_private.wrappedFilter, &filterValue);

   int16_t scaledFilterValue = (int16_t)(filterValue / instance->_private.scalingFactor);
   memcpy(value, &scaledFilterValue, sizeof(int16_t));
}

static void Reset(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Scaled_t *);
   Filter_Reset(instance->_private.wrappedFilter);
}

static I_Input_t *GetReadyInput(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Scaled_t *);
   return Filter_GetReadyInput(instance->_private.wrappedFilter);
}

static const I_Filter_Api_t api = { Seed, Feed, Read, Reset, GetReadyInput };

void Filter_Scaled_Init(
   Filter_Scaled_t *instance,
   I_Filter_t *wrappedFilter,
   uint8_t scalingFactor)
{
   instance->_private.wrappedFilter = wrappedFilter;
   instance->_private.scalingFactor = scalingFactor;
   instance->interface.api = &api;
}

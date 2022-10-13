/*!
 * @file
 * @brief Implementation of long term average filter
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Filter_LongTermAverage.h"
#include "Constants_Binary.h"
#include "utils.h"
#include "uassert.h"
#include <string.h>

static void Seed(I_Filter_t *_instance, const void *seedValue)
{
   REINTERPRET(instance, _instance, Filter_LongTermAverage_t *);

   memcpy(&instance->_private.average1, seedValue, sizeof(instance->_private.average1));
   memcpy(&instance->_private.average2, seedValue, sizeof(instance->_private.average2));
   memcpy(&instance->_private.average3, seedValue, sizeof(instance->_private.average3));

   Event_Synchronous_Publish(&instance->_private.onReady, on);
}

static void Feed(I_Filter_t *_instance, const void *_newValue)
{
   REINTERPRET(instance, _instance, Filter_LongTermAverage_t *);
   const int16_t *newValue = _newValue;

   instance->_private.average3 += (*newValue - instance->_private.average3) / instance->_private.beta;
   instance->_private.average2 += (instance->_private.average3 - instance->_private.average2) / instance->_private.beta;
   instance->_private.average1 += (instance->_private.average2 - instance->_private.average1) / instance->_private.beta;
}

static void Read(I_Filter_t *_instance, void *value)
{
   REINTERPRET(instance, _instance, Filter_LongTermAverage_t *);

   memcpy(value, &instance->_private.average1, sizeof(instance->_private.average1));
}

static void Reset(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_LongTermAverage_t *);
   instance->_private.average1 = 0;
   instance->_private.average2 = 0;
   instance->_private.average3 = 0;

   Event_Synchronous_Publish(&instance->_private.onReady, off);
}

static I_Input_t *GetReadyInput(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_LongTermAverage_t *);
   return &instance->_private.readyInput;
}

static const I_Filter_Api_t api = { Seed, Feed, Read, Reset, GetReadyInput };

static bool Ready(Filter_LongTermAverage_t *instance)
{
   return (instance->_private.average1 != 0);
}

static void ReadInput(I_Input_t *_instance, void *value)
{
   Filter_LongTermAverage_t *instance = CONTAINER_OF(Filter_LongTermAverage_t, _private.readyInput, _instance);
   *(bool *)value = Ready(instance);
}

static I_Event_t *GetOnChangeEvent(I_Input_t *_instance)
{
   Filter_LongTermAverage_t *instance = CONTAINER_OF(Filter_LongTermAverage_t, _private.readyInput, _instance);
   return &instance->_private.onReady.interface;
}

static const I_Input_Api_t inputApi = { .Read = ReadInput, .GetOnChangeEvent = GetOnChangeEvent };

void Filter_LongTermAverage_Init(
   Filter_LongTermAverage_t *instance,
   uint8_t beta)
{
   uassert(beta > 0);

   Event_Synchronous_Init(&instance->_private.onReady);
   instance->_private.readyInput.api = &inputApi;

   instance->_private.beta = beta;
   instance->interface.api = &api;
}

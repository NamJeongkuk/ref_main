/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Filter_Fallback.h"
#include "Constants_Binary.h"
#include "uassert.h"
#include "utils.h"

static bool SensorIsValid(Filter_Fallback_t *instance)
{
   return instance->_private.isValid;
}

static bool FallbackFilterIsReady(Filter_Fallback_t *instance)
{
   bool isReady;
   InputOutput_Read(&instance->_private.readyInputOutput.interface, &isReady);
   return isReady;
}

static bool ValueIsValid(Filter_Fallback_t *instance, int16_t value)
{
   return value != instance->_private.invalidValueDegFx100;
}

static void Seed(I_Filter_t *_instance, const void *_seedValue)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);
   const int16_t *seedValue = _seedValue;

   if(ValueIsValid(instance, *seedValue))
   {
      instance->_private.badReadingCounter = 0;
      instance->_private.isValid = true;
      Filter_Seed(instance->_private.wrappedFilter, seedValue);
   }
   else
   {
      instance->_private.goodReadingCounter = 0;
      instance->_private.isValid = false;
      Filter_Reset(instance->_private.wrappedFilter);
   }

   InputOutput_Write(&instance->_private.readyInputOutput.interface, set);
}

static void Feed(I_Filter_t *_instance, const void *_newValue)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);
   const int16_t *feedValue = _newValue;
   bool reseeded = false;

   if(ValueIsValid(instance, *feedValue))
   {
      instance->_private.badReadingCounter = 0;

      if(!SensorIsValid(instance))
      {
         instance->_private.goodReadingCounter++;

         if(instance->_private.goodReadingCounter == instance->_private.goodReadingCounterMax)
         {
            instance->_private.isValid = true;
            reseeded = true;

            Filter_Seed(instance->_private.wrappedFilter, feedValue);
         }
      }
   }
   else
   {
      instance->_private.goodReadingCounter = 0;

      if(SensorIsValid(instance))
      {
         instance->_private.badReadingCounter++;

         if(instance->_private.badReadingCounter == instance->_private.badReadingCounterMax)
         {
            instance->_private.isValid = false;
         }
      }
   }

   if(SensorIsValid(instance) &&
      ValueIsValid(instance, *feedValue) &&
      !reseeded)
   {
      Filter_Feed(instance->_private.wrappedFilter, feedValue);
   }
}

static void Read(I_Filter_t *_instance, void *value)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);

   uassert(FallbackFilterIsReady(instance));

   Filter_Fallback_Data_t filterData;
   filterData.isValid = SensorIsValid(instance);
   filterData.filteredValue = instance->_private.fallbackValueDegFx100;

   if(SensorIsValid(instance))
   {
      Filter_Read(instance->_private.wrappedFilter, &filterData.filteredValue);
   }

   memcpy(value, &filterData, sizeof(Filter_Fallback_Data_t));
}

static void Reset(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);

   instance->_private.goodReadingCounter = 0;
   instance->_private.badReadingCounter = 0;
   instance->_private.isValid = false;

   InputOutput_Write(&instance->_private.readyInputOutput.interface, clear);
   Filter_Reset(instance->_private.wrappedFilter);
}

static I_Input_t *GetReadyInput(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);
   return InputOutput_AsInput(&instance->_private.readyInputOutput);
}

static const I_Filter_Api_t api = { Seed, Feed, Read, Reset, GetReadyInput };

void Filter_Fallback_Init(
   Filter_Fallback_t *instance,
   I_Filter_t *wrappedFilter,
   I_DataModel_t *dataModel,
   int16_t invalidValueDegFx100,
   int16_t fallbackValueDegFx100,
   uint16_t goodReadingCounterMax,
   uint16_t badReadingCounterMax)
{
   instance->_private.wrappedFilter = wrappedFilter;
   instance->_private.dataModel = dataModel;
   instance->_private.invalidValueDegFx100 = invalidValueDegFx100;
   instance->_private.fallbackValueDegFx100 = fallbackValueDegFx100;
   instance->_private.goodReadingCounterMax = goodReadingCounterMax;
   instance->_private.badReadingCounterMax = badReadingCounterMax;
   instance->_private.goodReadingCounter = 0;
   instance->_private.badReadingCounter = 0;
   instance->_private.isValid = false;

   InputOutput_Simple_Init(
      &instance->_private.readyInputOutput,
      &instance->_private.isReady,
      sizeof(instance->_private.isReady));

   instance->interface.api = &api;
}

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Filter_Fallback.h"
#include "Constants_Binary.h"
#include "utils.h"

static bool ThermistorIsValid(Filter_Fallback_t *instance)
{
   bool thermistorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.validThermistorErd,
      &thermistorIsValid);

   return thermistorIsValid;
}

static bool WrappedFilterIsReady(Filter_Fallback_t *instance)
{
   I_Input_t *readyInput = Filter_GetReadyInput(instance->_private.wrappedFilter);
   bool wrappedFilterIsReady;
   Input_Read(readyInput, &wrappedFilterIsReady);

   return wrappedFilterIsReady;
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

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.validThermistorErd,
         set);

      Filter_Seed(instance->_private.wrappedFilter, seedValue);
   }
   else
   {
      instance->_private.goodReadingCounter = 0;

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.validThermistorErd,
         clear);
   }
}

static void Feed(I_Filter_t *_instance, const void *_newValue)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);

   const int16_t *feedValue = _newValue;

   if(ValueIsValid(instance, *feedValue))
   {
      instance->_private.badReadingCounter = 0;

      if(!ThermistorIsValid(instance))
      {
         instance->_private.goodReadingCounter++;

         if(instance->_private.goodReadingCounter == instance->_private.goodReadingCounterMax)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.validThermistorErd,
               set);

            instance->_private.reseeded = true;

            Filter_Seed(instance->_private.wrappedFilter, feedValue);
         }
      }
   }
   else
   {
      instance->_private.goodReadingCounter = 0;

      if(ThermistorIsValid(instance))
      {
         instance->_private.badReadingCounter++;

         if(instance->_private.badReadingCounter == instance->_private.badReadingCounterMax)
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.validThermistorErd,
               clear);
         }
      }
   }

   if(ThermistorIsValid(instance) &&
      *feedValue != instance->_private.invalidValueDegFx100 &&
      !instance->_private.reseeded)
   {
      Filter_Feed(instance->_private.wrappedFilter, feedValue);
   }

   instance->_private.reseeded = false;
}

static void Read(I_Filter_t *_instance, void *value)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);

   if(WrappedFilterIsReady(instance))
   {
      if(!ThermistorIsValid(instance))
      {
         memcpy(value, &instance->_private.fallbackValueDegFx100, sizeof(int16_t));
      }
      else
      {
         int16_t filterValue;
         Filter_Read(instance->_private.wrappedFilter, &filterValue);

         memcpy(value, &filterValue, sizeof(int16_t));
      }
   }
   else
   {
      memcpy(value, &instance->_private.fallbackValueDegFx100, sizeof(int16_t));
   }
}

static void Reset(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);

   Filter_Reset(instance->_private.wrappedFilter);
}

static I_Input_t *GetReadyInput(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Fallback_t *);

   return Filter_GetReadyInput(instance->_private.wrappedFilter);
}

static const I_Filter_Api_t api = { Seed, Feed, Read, Reset, GetReadyInput };

void Filter_Fallback_Init(
   Filter_Fallback_t *instance,
   I_Filter_t *wrappedFilter,
   I_DataModel_t *dataModel,
   int16_t invalidValueDegFx100,
   int16_t fallbackValueDegFx100,
   uint16_t goodReadingCounterMax,
   uint16_t badReadingCounterMax,
   Erd_t validThermistorErd)
{
   instance->_private.wrappedFilter = wrappedFilter;
   instance->_private.dataModel = dataModel;
   instance->_private.invalidValueDegFx100 = invalidValueDegFx100;
   instance->_private.fallbackValueDegFx100 = fallbackValueDegFx100;
   instance->_private.goodReadingCounterMax = goodReadingCounterMax;
   instance->_private.badReadingCounterMax = badReadingCounterMax;
   instance->_private.goodReadingCounter = 0;
   instance->_private.badReadingCounter = 0;
   instance->_private.validThermistorErd = validThermistorErd;
   instance->_private.reseeded = false;

   instance->interface.api = &api;
}

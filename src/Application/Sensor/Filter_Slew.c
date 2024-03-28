/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Filter_Slew.h"
#include "utils.h"

static void Seed(I_Filter_t *_instance, const void *_seedValue)
{
   REINTERPRET(instance, _instance, Filter_Slew_t *);
   instance->_private.hasAPreviousValue = false;

   const int16_t *seedValue = _seedValue;
   Filter_Seed(instance->_private.wrappedFilter, seedValue);
}

static void Feed(I_Filter_t *_instance, const void *_newValue)
{
   REINTERPRET(instance, _instance, Filter_Slew_t *);

   const int16_t *feedValue = _newValue;
   Filter_Feed(instance->_private.wrappedFilter, feedValue);
}

static void Read(I_Filter_t *_instance, void *value)
{
   REINTERPRET(instance, _instance, Filter_Slew_t *);

   int16_t filterValue;
   Filter_Read(instance->_private.wrappedFilter, &filterValue);

   if(instance->_private.hasAPreviousValue)
   {
      int16_t slewFeedValue;
      if((instance->_private.previousWrappedFilterValue - filterValue) > instance->_private.slewLimitx100)
      {
         slewFeedValue = instance->_private.previousWrappedFilterValue - instance->_private.slewLimitx100;
      }
      else if((filterValue - instance->_private.previousWrappedFilterValue) > instance->_private.slewLimitx100)
      {
         slewFeedValue = instance->_private.previousWrappedFilterValue + instance->_private.slewLimitx100;
      }
      else
      {
         slewFeedValue = filterValue;
      }

      instance->_private.previousWrappedFilterValue = slewFeedValue;

      memcpy(value, &slewFeedValue, sizeof(int16_t));
   }
   else
   {
      instance->_private.previousWrappedFilterValue = filterValue;
      instance->_private.hasAPreviousValue = true;

      memcpy(value, &filterValue, sizeof(int16_t));
   }
}

static void Reset(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Slew_t *);

   Filter_Reset(instance->_private.wrappedFilter);
}

static I_Input_t *GetReadyInput(I_Filter_t *_instance)
{
   REINTERPRET(instance, _instance, Filter_Slew_t *);

   return Filter_GetReadyInput(instance->_private.wrappedFilter);
}

static const I_Filter_Api_t api = { Seed, Feed, Read, Reset, GetReadyInput };

void Filter_Slew_Init(
   Filter_Slew_t *instance,
   I_Filter_t *wrappedFilter,
   uint16_t slewLimitx100)
{
   instance->_private.wrappedFilter = wrappedFilter;
   instance->_private.slewLimitx100 = slewLimitx100;
   instance->_private.hasAPreviousValue = false;

   instance->interface.api = &api;
}

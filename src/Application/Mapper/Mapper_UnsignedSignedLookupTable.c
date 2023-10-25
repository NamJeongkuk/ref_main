/*!
 * @file
 * @brief
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "Mapper_UnsignedSignedLookupTable.h"
#include "utils.h"

static int64_t Interpolatation(int64_t x, const Mapper_UnsignedSignedLookupTableMapping_t *mappings, size_t mappingCount)
{
   size_t i;
   for(i = 0; i < (mappingCount - 1); i++)
   {
      if(x == mappings[i].x)
      {
         return mappings[i].y;
      }
      else if(x < mappings[i + 1].x)
      {
         int32_t x0 = mappings[i].x;
         int32_t x1 = mappings[i + 1].x;
         int32_t y0 = mappings[i].y;
         int32_t y1 = mappings[i + 1].y;

         return (y0 + (((y1 - y0) * (x - x0)) / (x1 - x0)));
      }
   }

   return 0;
}

static int64_t GetMappedValue(I_Mapper_t *_instance, int64_t x)
{
   REINTERPRET(instance, _instance, Mapper_UnsignedSignedLookupTable_t *);
   const Mapper_UnsignedSignedLookupTableMapping_t *mappings = instance->_private.configuration->mappings;
   size_t mappingCount = instance->_private.configuration->mappingCount;

   if(x <= mappings[0].x)
   {
      return mappings[0].y;
   }
   else if(x >= mappings[mappingCount - 1].x)
   {
      return mappings[mappingCount - 1].y;
   }
   else
   {
      return Interpolatation(x, mappings, mappingCount);
   }
}

static uint16_t GetValueIndex(I_Mapper_t *_instance, int64_t x)
{
   REINTERPRET(instance, _instance, Mapper_UnsignedSignedLookupTable_t *);

   const Mapper_UnsignedSignedLookupTableMapping_t *mappings = instance->_private.configuration->mappings;
   size_t mappingCount = instance->_private.configuration->mappingCount;
   size_t entry;

   for(entry = (mappingCount - 1); entry > 0; entry--)
   {
      if(x >= mappings[entry].x)
      {
         return entry;
      }
   }

   // Default to returning an index of 0
   return 0;
}

static const I_Mapper_Api_t api = { GetMappedValue, GetValueIndex };

void Mapper_UnsignedSignedLookupTable_Init(
   Mapper_UnsignedSignedLookupTable_t *instance,
   const Mapper_UnsignedSignedLookupTableConfiguration_t *configuration)
{
   instance->interface.api = &api;
   instance->_private.configuration = configuration;
}

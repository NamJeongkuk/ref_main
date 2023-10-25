/*!
 * @file
 * @brief Mapper which has an unsigned raw value and a signed mapped value lookup table
 * The mapper will return mapped value from the lookup table
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef MAPPER_UNSIGNEDSIGNEDLOOKUPTABLE_H
#define MAPPER_UNSIGNEDSIGNEDLOOKUPTABLE_H

#include "I_Mapper.h"
#include <stddef.h>

typedef struct
{
   uint16_t x;
   int16_t y;
} Mapper_UnsignedSignedLookupTableMapping_t;

typedef struct
{
   const Mapper_UnsignedSignedLookupTableMapping_t *mappings;
   size_t mappingCount;
} Mapper_UnsignedSignedLookupTableConfiguration_t;

typedef struct
{
   I_Mapper_t interface;

   struct
   {
      const Mapper_UnsignedSignedLookupTableConfiguration_t *configuration;
   } _private;
} Mapper_UnsignedSignedLookupTable_t;

/*!
 * Initialize a lookup table mapper.
 * @param instance The Mapper.
 * @param configuration Contains the lookup table.
 */
void Mapper_UnsignedSignedLookupTable_Init(
   Mapper_UnsignedSignedLookupTable_t *instance,
   const Mapper_UnsignedSignedLookupTableConfiguration_t *configuration);

#endif

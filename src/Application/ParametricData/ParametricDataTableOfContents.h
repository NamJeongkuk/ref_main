/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PARAMETRICDATATABLEOFCONTENTS_H
#define PARAMETRICDATATABLEOFCONTENTS_H

#include <stdint.h>
#include "PersonalityParametricData.h"
#include "ImageHeader.h"

typedef struct
{
   const uint8_t numberOfPersonalities;
   const PersonalityParametricData_t *const *const personalities;
} ParametricDataTableOfContents_t;

#endif

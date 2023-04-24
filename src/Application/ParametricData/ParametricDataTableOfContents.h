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
#include "ShortGitHash.h"

typedef struct
{
   const ShortGitHash_t shortGitHash;
   const uint8_t numberOfPersonalities;
   const PersonalityParametricData_t *const *const personalities;
} ParametricDataTableOfContents_t;

#endif

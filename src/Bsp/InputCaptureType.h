/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUTCAPTURETYPE_H
#define INPUTCAPTURETYPE_H

#include <stdint.h>

typedef uint32_t InputCaptureMicroSeconds_t;

#define SWAP_DEFINITION_InputCaptureMicroSeconds_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

typedef uint32_t InputCaptureCounts_t;

#define SWAP_DEFINITION_InputCaptureCounts_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHDEBUGRAWCOUNTS_H
#define CAPTOUCHDEBUGRAWCOUNTS_H

#include <stdint.h>
#include "CapTouchKeys.h"
#include "utils.h"

#define EXPAND_AS_KEY_FIELDS(keyId) \
   uint16_t CONCAT(key, keyId);

typedef struct
{
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_FIELDS)
} CapTouchDebugRawCounts_t;

#undef EXPAND_AS_KEY_FIELDS

#endif

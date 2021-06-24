/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHDEBUGDELTAS_H
#define CAPTOUCHDEBUGDELTAS_H

#include <stdint.h>
#include "CapTouchKeys.h"

#define EXPAND_AS_KEY_FIELDS(keyId) \
   uint16_t CONCAT(key, keyId);

typedef struct
{
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_FIELDS)
} CapTouchDebugDeltas_t;

#undef EXPAND_AS_KEY_FIELDS

#endif

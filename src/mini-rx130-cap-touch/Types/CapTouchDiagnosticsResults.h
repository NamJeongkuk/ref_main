/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHDIAGNOSTICSRESULTS_H
#define CAPTOUCHDIAGNOSTICSRESULTS_H

#include <stdint.h>
#include "CapTouchKeys.h"
#include "utils.h"

#define EXPAND_AS_KEY_FIELDS(keyId) \
   uint8_t CONCAT(key, keyId);

typedef struct
{
   SAFETY_CRITICAL_CAP_TOUCH_KEYS(EXPAND_AS_KEY_FIELDS)
} CapTouchDiagnosticsResults_t;

#undef EXPAND_AS_KEY_FIELDS

#endif

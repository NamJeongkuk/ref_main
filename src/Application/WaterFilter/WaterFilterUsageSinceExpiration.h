/*!
 * @file
 * @brief The type for ERD 0x1238
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERUSAGESINCEEXPIRATION_H
#define WATERFILTERUSAGESINCEEXPIRATION_H

#include <stdint.h>

typedef struct
{
   uint16_t daysSinceFilterExpired;
   uint16_t dispensesSinceFilterExpired;
} WaterFilterUsageSinceExpiration_t;

#define SWAP_DEFINITION_WaterFilterUsageSinceExpiration_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, daysSinceFilterExpired),                 \
      SWAPPED_FIELD(_name, _type, dispensesSinceFilterExpired),

#endif

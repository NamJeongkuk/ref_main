/*!
 * @file
 * @brief Structure used to access the ConstArrayMap that contains the state based votes.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGMAPKEY_H
#define COOLINGMAPKEY_H

#include "CoolingMode.h"
#include "CoolingSpeed.h"

typedef struct
{
   CoolingSpeed_t speed;
   CoolingMode_t mode;
} CoolingMapKey_t;

#endif

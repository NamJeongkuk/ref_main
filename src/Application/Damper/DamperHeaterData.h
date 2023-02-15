/*!
 * @file
 * @brief damper heater parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERHEATERDATA_H
#define DAMPERHEATERDATA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   bool damperHeaterOnWhenDefrostHeaterIsOnEnable;
   uint8_t damperHeaterDefrostOnTimeInMinutes;
} DamperHeaterData_t;

#endif

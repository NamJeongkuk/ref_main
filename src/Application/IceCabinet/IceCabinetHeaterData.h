/*!
 * @file
 * @brief ice cabinet heater parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETHEATERDATA_H
#define ICECABINETHEATERDATA_H

#include <stdint.h>
#include "PercentageDutyCycle.h"

typedef struct
{
   PercentageDutyCycle_t iceCabinetHeaterMinDutyCyclePercentage;
   int16_t iceCabinetHeaterInterceptCoefficient;
   int16_t iceCabinetHeaterIceFormationModeCoefficient;
   int16_t iceCabinetHeaterIceCabinetFanCoefficient;
   int16_t iceCabinetHeaterFreezerSetpointSlopeCoefficient;
} IceCabinetHeaterData_t;

#endif

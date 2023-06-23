/*!
 * @file
 * @brief cabinet offset parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CABINETOFFSETDATA_H
#define CABINETOFFSETDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t freshFoodOffsetInDegFx100;
   TemperatureDegFx100_t freezerOffsetInDegFx100;
   TemperatureDegFx100_t iceBoxOffsetInDegFx100;
} CabinetOffsetData_t;

#endif

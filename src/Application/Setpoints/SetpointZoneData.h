/*!
 * @file
 * @brief set point zone parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTZONEDATA_H
#define SETPOINTZONEDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t freezerColdSetpointZoneLimitInDegFx100;
   TemperatureDegFx100_t freezerWarmSetpointZoneLimitInDegFx100;
   TemperatureDegFx100_t freshFoodColdSetpointZoneLimitInDegFx100;
   TemperatureDegFx100_t freshFoodWarmSetpointZoneLimitInDegFx100;
   TemperatureDegFx100_t convertibleCompartmentSetpointZoneLimitInDegFx100;
} SetpointZoneData_t;

#endif

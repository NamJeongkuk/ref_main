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
   TemperatureDegFx100_t freezerTriggerColdSetpointZoneInDegFx100;
   TemperatureDegFx100_t freezerTriggerWarmSetpointZoneInDegFx100;
   TemperatureDegFx100_t freshFoodTriggerColdSetpointZoneInDegFx100;
   TemperatureDegFx100_t freshFoodTriggerWarmSetpointZoneInDegFx100;
   TemperatureDegFx100_t convertibleCompartmentTriggerSetpointZoneLimitInDegFx100;
} SetpointZoneData_t;

#endif

/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOLDHEATERCONTROLREQUEST_H
#define ICEMAKERMOLDHEATERCONTROLREQUEST_H

#include "TemperatureDegFx100.h"

typedef struct
{
   bool enable;
   bool skipInitialOnTime;
   TemperatureDegFx100_t onTemperatureInDegFx100;
   TemperatureDegFx100_t offTemperatureInDegFx100;
} IceMakerMoldHeaterControlRequest_t;

#endif

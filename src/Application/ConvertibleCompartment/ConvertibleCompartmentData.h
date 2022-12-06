/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTDATA_H
#define CONVERTIBLECOMPARTMENTDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   bool hasConvertibleCompartment;
   TemperatureDegFx100_t convertibleCompartmentThresholdDegFx100;
} ConvertibleCompartmentData_t;

#endif

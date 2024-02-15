/*!
 * @file
 * @brief damper parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERDATA_H
#define DAMPERDATA_H

#include <stdint.h>

typedef struct
{
   uint16_t stepsToOpen;
   uint16_t stepsToClose;
   uint16_t stepsToHome;
   uint8_t delayBetweenStepEventsInHundredsOfMicroseconds;
   uint8_t maxTimeForDamperToBeOpenInMinutes;
   uint8_t targetCompartmentMinimumTemperatureChangeTimeInMinutes;
   uint8_t targetCompartmentDamperHeaterOnTimeInMinutes;
   int16_t sourceCompartmentMaximumTemperatureToRunCheckInDegFx100;
   int16_t targetCompartmentMinimumTemperatureChangeInDegFx100;
} DamperData_t;

#endif

/*!
 * @file
 * @brief single damper parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDAMPERDATA_H
#define SINGLEDAMPERDATA_H

#include <stdint.h>

typedef struct
{
   uint16_t stepsToOpen;
   uint16_t stepsToClose;
   uint16_t stepsToHome;
   uint8_t delayBetweenStepEventsInMs;
   uint8_t maxTimeForDamperToBeOpenInMinutes;
   uint8_t targetCompartmentMinimumTemperatureChangeTimeInMinutes;
   uint8_t targetCompartmentDamperHeaterOnTimeInMinutes;
   int16_t sourceCompartmentMaximumTemperatureToRunCheckInDegFx100;
   int16_t targetCompartmentMinimumTemperatureChangeInDegFx100;
} SingleDamperData_t;

#endif

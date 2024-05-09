/*!
 * @file
 * @brief damper parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERDATA_H
#define DAMPERDATA_H

#include <stdint.h>

enum
{
   DamperId_NormalDamper,
   DamperId_ConvertibleCompartmentDamper
};
typedef uint8_t DamperId_t;

typedef struct
{
   uint16_t stepsToOpen;
   uint16_t stepsToClose;
} DamperStepData_t;

typedef struct
{
   DamperStepData_t damperStepsAsFreshFood;
   DamperStepData_t damperStepsAsFreezer;
} ConvertibleDamperStepData_t;

typedef struct
{
   DamperId_t damperId;
   union
   {
      DamperStepData_t *damperStepData;
      ConvertibleDamperStepData_t *convertibleDamperStepData;
   };
   uint16_t stepsToHome;
   uint8_t delayBetweenStepEventsInHundredsOfMicroseconds;
   uint16_t maxTimeForDamperToBeOpenInMinutes;
   uint16_t maxTimeForDamperToBeClosedInMinutes;
   uint8_t targetCompartmentMinimumTemperatureChangeTimeInMinutes;
   uint8_t targetCompartmentDamperHeaterOnTimeInMinutes;
   int16_t sourceCompartmentMaximumTemperatureToRunCheckInDegFx100;
   int16_t targetCompartmentMinimumTemperatureChangeInDegFx100;
} DamperData_t;

#endif

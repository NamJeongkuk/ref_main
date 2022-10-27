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
   uint16_t maxTimeForDamperToBeOpenInMinutes;
} SingleDamperData_t;

#endif

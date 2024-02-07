/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SEALEDSYSTEMVALVEDATA_H
#define SEALEDSYSTEMVALVEDATA_H

#include <stdint.h>

typedef struct
{
   uint8_t positionAStep;
   uint8_t positionBStep;
   uint8_t positionCStep;
   uint8_t positionDStep;
   uint16_t numberOfStepsToHome;
   uint16_t excitationDelayInMilliseconds;
   uint8_t delayBetweenStepEventsInMilliseconds;
} SealedSystemValveData_t;

#endif

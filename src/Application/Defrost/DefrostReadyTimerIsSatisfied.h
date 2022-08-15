/*!
 * @file
 * @brief Determines whether or not the defrost timer is satisfied
 * based on the compressor on time and door accelerations
 * compared to the time when defrost timer is satisfied.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTREADYTIMERISSATISFIED_H
#define DEFROSTREADYTIMERISSATISFIED_H

#include "I_DataModel.h"
#include "DefrostData.h"

typedef struct
{
   Erd_t compressorOnTimeInSecondsErd; // uint32_t
   Erd_t freshFoodScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t freezerScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t convertibleCompartmentScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t defrostReadyTimerIsSatisfiedErd; // bool
   Erd_t timeInMinutesWhenDefrostReadyTimerIsSatisfiedErd; // uint16_t
   Erd_t defrostCompressorOnTimeCounterReadyErd; // bool
   Erd_t doorAccelerationCounterReadyErd; // bool
} DefrostReadyTimerIsSatisfiedConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const DefrostReadyTimerIsSatisfiedConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
   } _private;
} DefrostReadyTimerIsSatisfied_t;

void DefrostReadyTimerIsSatisfied_Init(
   DefrostReadyTimerIsSatisfied_t *instance,
   I_DataModel_t *dataModel,
   const DefrostReadyTimerIsSatisfiedConfiguration_t *config);

#endif

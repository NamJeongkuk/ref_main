/*!
 * @file
 * @brief Determines whether or not the defrost timer is satisfied
 * based on the compressor on time and door accelerations
 * compared to the time when defrost timer is satisfied.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTIMERISSATISFIED_H
#define DEFROSTTIMERISSATISFIED_H

#include "I_DataModel.h"
#include "DefrostData.h"

typedef struct
{
   Erd_t compressorOnTimeInSecondsErd; // uint32_t
   Erd_t freshFoodDoorAccelerationCountErd; // uint32_t
   Erd_t freezerDoorAccelerationCountErd; // uint32_t
   Erd_t convertibleCompartmentDoorAccelerationCountErd; // uint32_t
   Erd_t defrostTimerIsSatisfiedErd; // bool
   Erd_t timeInMinutesWhenDefrostTimerIsSatisfiedErd; // uint16_t
} DefrostTimerIsSatisfiedConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const DefrostTimerIsSatisfiedConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
   } _private;
} DefrostTimerIsSatisfied_t;

void DefrostTimerIsSatisfied_Init(
   DefrostTimerIsSatisfied_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTimerIsSatisfiedConfiguration_t *config);

#endif

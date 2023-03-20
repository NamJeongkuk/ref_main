/*!
 * @file
 * @brief Determines if we're ready for defrost
 * based on the compressor on time and door accelerations
 * compared to the time until defrost timer is satisfied.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef READYTODEFROST_H
#define READYTODEFROST_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "Fsm.h"

typedef struct
{
   Erd_t defrostCompressorOnTimeInSecondsErd; // uint32_t
   Erd_t freshFoodScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t freezerScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t convertibleCompartmentScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t readyToDefrostErd; // bool
   Erd_t defrostCompressorOnTimeCounterReadyErd; // bool
   Erd_t doorAccelerationCounterReadyErd; // bool
   Erd_t freezerFilteredTemperatureWasTooWarmAtPowerUpErd; // bool
   Erd_t useMinimumReadyToDefrostTimeErd; // bool
   Erd_t invalidFreezerEvaporatorThermistorDuringDefrostErd; // bool
   Erd_t freshFoodDefrostWasAbnormalErd; // bool
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t hasConvertibleCompartment; // bool
   Erd_t eepromClearedErd; // bool
   Erd_t waitingForDefrostTimeInSecondsErd; // uint32_t
   Erd_t waitingToDefrostErd; // bool
} ReadyToDefrostConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      EventSubscription_t dataModelSubscription;
      const ReadyToDefrostConfiguration_t *config;
      uint32_t timeBetweenDefrostsInMinutes;
      const DefrostData_t *defrostData;
   } _private;
} ReadyToDefrost_t;

void ReadyToDefrost_Init(
   ReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostConfiguration_t *config,
   const DefrostData_t *defrostData);

#endif
